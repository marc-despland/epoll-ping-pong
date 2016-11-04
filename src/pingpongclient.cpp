#include "pingpongclient.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include "log.h"
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>

#define MAXEVENTS 64

PingPongClient::PingPongClient(string host, int port, int size)throw(ConnectionPoolException):Runnable()  {
	this->host=host;
	this->port=port;
	this->size=size;
	this->clientfd=0;
	this->events = (struct epoll_event *) calloc (MAXEVENTS, sizeof(struct epoll_event));
	this->pool = epoll_create1 (0);
	if (this->pool == -1) {
		throw ConnectionPoolException("Epoll create1 failed");
	}
	this->hasSource=false;
}


void PingPongClient::setSourceRange(string ip, int i) {
	long hostaddress;
	struct hostent *serverent;
	bzero(&(this->source),sizeof(struct sockaddr_in));
	hostaddress = inet_addr(ip.c_str());
	hostaddress+=i*256*256*256;
	if ( (long)hostaddress != (long)4294967295) {
		Log::logger->log("CNXTCP",DEBUG) << host.c_str() << " is an IP address : "<< hostaddress <<endl;
		bcopy(&hostaddress,&(this->source).sin_addr,sizeof(hostaddress));
		this->source.sin_port = 0;
		this->source.sin_family = AF_INET;
		this->hasSource=true;
	} else {
		Log::logger->log("CNXTCP",DEBUG) << host.c_str() << " is probably an host name" <<endl;
	}
}


void PingPongClient::setSource(string host) {
	long hostaddress;
	struct hostent *serverent;
	bzero(&(this->source),sizeof(struct sockaddr_in));
	hostaddress = inet_addr(host.c_str());
	if ( (long)hostaddress != (long)4294967295) {
		Log::logger->log("CNXTCP",DEBUG) << host.c_str() << " is an IP address : "<< hostaddress <<endl;
		bcopy(&hostaddress,&(this->source).sin_addr,sizeof(hostaddress));
	} else {
		Log::logger->log("CNXTCP",DEBUG) << host.c_str() << " is probably an host name" <<endl;
		serverent = gethostbyname(host.c_str());
		if (serverent == NULL){
			Log::logger->log("CNXTCP",ERROR) << this->host.c_str() << " can't find its IP address" <<endl;
			throw CantConnectException(strerror(errno));
		}
		bcopy(serverent->h_addr,&(this->source.sin_addr),serverent->h_length);
	}
	this->source.sin_port = 0;
	this->source.sin_family = AF_INET;
	this->hasSource=true;
}


void PingPongClient::connect() throw(CantConnectException) {
	struct sockaddr_in server;
	long hostaddress;
	struct hostent *serverent;
	
	bzero(&server,sizeof(server));
	hostaddress = inet_addr(this->host.c_str());
	if ( (long)hostaddress != (long)4294967295) {
		Log::logger->log("CNXTCP",DEBUG) << this->host.c_str() << " is an IP address : "<< hostaddress <<endl;
		bcopy(&hostaddress,&server.sin_addr,sizeof(hostaddress));
	} else {
		Log::logger->log("CNXTCP",DEBUG) << this->host.c_str() << " is probably an host name" <<endl;
		serverent = gethostbyname(this->host.c_str());
		if (serverent == NULL){
			Log::logger->log("CNXTCP",ERROR) << this->host.c_str() << " can't find its IP address" <<endl;
			throw CantConnectException(strerror(errno));
		}
		bcopy(serverent->h_addr,&server.sin_addr,serverent->h_length);
	}
	server.sin_port = htons(this->port);
	server.sin_family = AF_INET;
	int socketfd;
	if ( (socketfd = ::socket(AF_INET,SOCK_STREAM,0)) < 0) {
		Log::logger->log("CNXTCP",ERROR) << "Can't connect : " << strerror(errno) <<endl;
		throw CantConnectException(strerror(errno));
	}
	if (this->hasSource) {
		::bind(socketfd, (struct sockaddr *)&(this->source), sizeof(struct sockaddr_in));
	}

	if (::connect( socketfd,(struct sockaddr *)&(server), sizeof(server)) < 0 ) {
		Log::logger->log("CNXTCP",ERROR) << "Can't connect : " << strerror(errno) <<endl;
		throw CantConnectException(strerror(errno));
	}
	PingPongClient::makeSocketNonBlocking(socketfd);
	struct epoll_event event;
	event.data.fd = socketfd;
  	event.events = EPOLLIN | EPOLLET;
  	int s = epoll_ctl (this->pool, EPOLL_CTL_ADD, socketfd, &event);
  	if (s == -1) {
  		Log::logger->log("CNXTCP",ERROR) << "Can't connect (epoll): " << strerror(errno) <<endl;
  		throw CantConnectException("Epoll ctl failed");
    }
    if (this->clientfd==0) this->clientfd=socketfd;
}






void PingPongClient::makeSocketNonBlocking(int socket) throw(MakeSocketNonBlockingException){
	int flags, s;
	flags = fcntl (socket, F_GETFL, 0);
	if (flags == -1) {
     throw MakeSocketNonBlockingException("Can't set to F_GETFL");
    }

	flags |= O_NONBLOCK;
	s = fcntl (socket, F_SETFL, flags);
	if (s == -1) {
      throw MakeSocketNonBlockingException("Can't set to F_SETFL");
    }
}

void PingPongClient::createConnections() {
	for(int i=0; i<this->size; i++) {
		this->connect();
		Log::logger->log("CNXTCP",DEBUG) << "Connecting Socket number " << i <<endl;
	}
}

void PingPongClient::run() {
	Log::logger->log("CNXTCP",NOTICE) << "Starting Ping Pong" <<endl;
	this->Runnable::start();
	char first[4];
	first[0]='b';
	first[1]='i';
	first[2]='p';
	first[3]=0;
	int nbread=0;
	write(this->clientfd, first, 4);
	while (this->Runnable::running()) {
		int n = epoll_wait (this->pool, this->events, MAXEVENTS, -1);
		for (int i = 0; i < n; i++) {
			if ((this->events[i].events & EPOLLERR) || (this->events[i].events & EPOLLHUP) || (!(this->events[i].events & EPOLLIN))) {
			//error
			} else { 
				ssize_t count;
                char buf[512];
                count = read (this->events[i].data.fd,  buf, sizeof buf);
                if (count<=0) {
                	::close(this->events[i].data.fd);
                } else {
                	nbread++;
                	if (nbread>100) {
                		nbread=0;
                		cout << ".";
                	}
                	//Log::logger->log("CNXTCP",DEBUG) << "Read on socket " << this->events[i].data.fd <<endl;
                	int ws=write(this->events[i].data.fd, buf, count);
                	if (ws<0) {
                		Log::logger->log("CNXTCP",ERROR) << "Can't write on socket: " << this->events[i].data.fd <<endl;
                	}
                }
			}
		}
	}

}

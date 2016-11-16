#include "pingpongserver.h"
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



PingPongServer::PingPongServer(int port, unsigned int size):Runnable() {
	this->port=port;
	this->size=size;
	this->count=0;
	this->first=true;
}


void PingPongServer::makeSocketNonBlocking(int socket) throw(MakeSocketNonBlockingException){
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

void PingPongServer::listen() throw(ConnectionListenException){
	struct sockaddr_in server;
	this->socketfd=::socket(AF_INET,SOCK_STREAM,0);

	bzero(&server,sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr=htonl(INADDR_ANY);
	server.sin_port=htons(this->port);
	if (::bind(this->socketfd,(struct sockaddr *)&server,sizeof(server))<0) {
		Log::logger->log("CNXTCP", DEBUG) << "Failed to bind the socket " << this->socketfd << " try to bind to " << this->port<<endl;
		throw ConnectionListenException("Can't bind socket");
	}
	try {
		PingPongServer::makeSocketNonBlocking(this->socketfd);
	} catch(MakeSocketNonBlockingException &e) {
		Log::logger->log("CNXTCP", DEBUG) << "Failed to make listen socket non blocking " << this->socketfd << " bind to " << this->port<<endl;
		throw ConnectionListenException("Can't make socket non blocking");
	}
	if (::listen(this->socketfd,this->size)<0) {
		Log::logger->log("CNXTCP", DEBUG) << "Failed to listen on socket " << this->socketfd << " bind to " << this->port<<endl;
		throw ConnectionListenException("Can't listen on socket");
	}
	this->pool=new ConnectionPool();
	this->pool->add(this->socketfd);
	Log::logger->log("CNXTCP", NOTICE) << "Listening on port " << this->port <<endl;
}


void PingPongServer::accept() throw(MakeSocketNonBlockingException) {
	Log::logger->log("CNXTCP", DEBUG) << "Wait for a new client" <<endl;
	int clientfd;
	struct sockaddr_in client;
	socklen_t clientsize;
	bzero(&client,sizeof(client));
	clientsize=sizeof(client);
	try {
		do {
			bzero(&client,sizeof(client));
			clientsize=sizeof(client);
			clientfd = ::accept(this->socketfd,(struct sockaddr *)&client,&clientsize);
			Log::logger->log("CNXTCP",DEBUG) << "Accept socket: " << clientfd << " error : "<< strerror(errno) <<endl;
			if (clientfd>=0) {
				if (this->first) {
					char first[4];
					first[0]='b';
					first[1]='i';
					first[2]='p';
					first[3]=0;
					write(clientfd, first, 4);
					this->first=false;
				}
				PingPongServer::makeSocketNonBlocking(clientfd);
				this->pool->add(clientfd);
				this->count++;
				Log::logger->log("CNXTCP", NOTICE) << "New Connection " << this->count <<endl;
			}
		} while ((clientfd>0) || (errno==EINTR));
	} catch(ConnectionPoolException &e) {
		Log::logger->log("CNXTCP", ERROR) << "A connection pool error occurs " <<endl;
	}
}


void PingPongServer::run() {
	this->listen();
	this->Runnable::start();
	while (this->Runnable::running()) {
		vector<int> * ready=this->pool->poll();
		for (unsigned int i=0; i<ready->size(); i++ ) {
			if (ready->at(i)==this->socketfd) {
				this->accept();
			} else {
				ssize_t nbb;
                char buf[512];
                nbb = read (ready->at(i),  buf, sizeof buf);
                if (nbb<=0) {
                	::close(ready->at(i));
                } else {
                	int newfd=this->pool->next(ready->at(i));
                	Log::logger->log("CNXTCP",DEBUG) << "Next socket: " << newfd <<endl;
                	int ws=write(newfd, buf, nbb);
                	if (ws<0) {
                		Log::logger->log("CNXTCP",ERROR) << "Can't write on socket: " << newfd <<endl;
                	}
                }
			}
		}
	}

}
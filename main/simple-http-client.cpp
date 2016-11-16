#include <iostream>
#include "options.h"
#include "log.h"
#include <string>
using namespace std;
#include "runnable.h"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>


void makeSocketNonBlocking(int socket) throw(exception){
	int flags, s;
	flags = fcntl (socket, F_GETFL, 0);
	if (flags == -1) {
     throw exception();
    }

	flags |= O_NONBLOCK;
	s = fcntl (socket, F_SETFL, flags);
	if (s == -1) {
      throw exception();
    }
}

#define MAXEVENTS 64

int main(int argc, char **argv) {
	Log::logger->setLevel(NOTICE);
	Options options(argv[0], "1.0.0", "Epoll Ping Pong https://github.com/marc-despland/epoll-ping-pong");
	try {
		options.add('d', "debug", "Start on debug mode", false, false);
		options.add('p', "port", "Port to connect to", true, true);
		options.add('s', "server", "Host to connect to", true, true);
		options.add('w', "wait", "wait before closing", false, false);
	} catch(ExistingOptionException &e ) {
	}
	try {
		options.parse(argc, argv);
		if (options.get('d')->isAssign()) Log::logger->setLevel(DEBUG);

		struct sockaddr_in server;
		long hostaddress;
		struct hostent *serverent;
		struct epoll_event *events;

		string host=options.get("server")->asChars();
		int port=options.get("port")->asInt();
		int		pool;
		pool = epoll_create1 (0);
		events = (struct epoll_event *) calloc (MAXEVENTS, sizeof(struct epoll_event));
		try {
			bzero(&server,sizeof(server));
			hostaddress = inet_addr(host.c_str());
			if ( (long)hostaddress != (long)4294967295) {
				Log::logger->log("CNXTCP",DEBUG) << host.c_str() << " is an IP address : "<< hostaddress <<endl;
				bcopy(&hostaddress,&server.sin_addr,sizeof(hostaddress));
			} else {
				Log::logger->log("CNXTCP",DEBUG) << host.c_str() << " is probably an host name" <<endl;
				serverent = gethostbyname(host.c_str());
				if (serverent == NULL){
					Log::logger->log("CNXTCP",ERROR) << host.c_str() << " can't find its IP address" <<endl;
					throw exception();
				}
				bcopy(serverent->h_addr,&server.sin_addr,serverent->h_length);
			}
			server.sin_port = htons(port);
			server.sin_family = AF_INET;
			int socketfd;
			if ( (socketfd = ::socket(AF_INET,SOCK_STREAM,0)) < 0) {
				Log::logger->log("CNXTCP",ERROR) << "Can't connect : " << strerror(errno) <<endl;
				throw exception();
			}
			if (::connect( socketfd,(struct sockaddr *)&(server), sizeof(server)) < 0 ) {
				Log::logger->log("CNXTCP",ERROR) << "Can't connect : " << strerror(errno) <<endl;
				throw exception();
			}
			makeSocketNonBlocking(socketfd);
			struct epoll_event event;
			event.data.fd = socketfd;
		  	event.events = EPOLLIN | EPOLLET;
		  	int s = epoll_ctl (pool, EPOLL_CTL_ADD, socketfd, &event);
		  	if (s == -1) {
		  		Log::logger->log("CNXTCP",ERROR) << "Can't connect (epoll): " << strerror(errno) <<endl;
		  		throw exception();
		    }
		    std::string request= "GET / HTTP1.1\r\nHost: "+host+"\r\nAccept: */*\r\n\r\n";
	  		int ws=write(socketfd, request.c_str(), request.length());
	        if (ws<0) {
	            Log::logger->log("CNXTCP",ERROR) << "Can't write on socket: " << socketfd <<endl;
	        }
	        int n = epoll_wait (pool, events, MAXEVENTS, -1);
			for (int i = 0; i < n; i++) {
				if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN))) {
				//error
				} else {
					Log::logger->log("CNXTCP",DEBUG) << "Reading on socket "<<endl;
					int readerror=0;
					ssize_t count;
					do {
						
                		char buf[512];
                		count=::read (events[i].data.fd,  buf, sizeof buf);
                		readerror=errno;
                		Log::logger->log("CNXTCP",DEBUG) << "Reading  on "<<events[i].data.fd<< " : "<<readerror << " : " << count<<endl;
                	} while ((readerror==0) && (count>0));
                	Log::logger->log("CNXTCP",DEBUG) << "Colsing socket "<<endl;
                	if (options.get('w')->isAssign()) sleep(2);
                	::close(events[i].data.fd);
                }
            }

		} catch(std::exception &e) {
			Log::logger->log("CNXTCP",ERROR) << "An error occors " << errno << " : " << strerror(errno) <<endl;
		}
		
	} catch (OptionsStopException &e) {
	} catch (UnknownOptionException &e) {
		cout << " Request unknown option"<<endl;
	}

}
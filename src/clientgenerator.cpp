#include "clientgenerator.h"
#include "connectionmanager.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "log.h"
#include <errno.h>
#include <strings.h>
#include <unistd.h>
//



ClientGenerator * ClientGenerator::singleton=new ClientGenerator();

ClientGenerator::ClientGenerator() {
	this->currenthost=0;
	this->remaininghost=0;
	this->remainingcntx=0;
	this->maxcntx=0;
	this->nbworker=0;
	this->workers=new std::vector<std::thread *>();
}

void ClientGenerator::start(std::string firsthost, int nbhost, int nbcntx, int nbworker, std::string destination, int port) {
	this->currenthost=inet_addr(firsthost.c_str());;
	this->remaininghost=nbhost-1;
	this->remainingcntx=nbcntx;
	this->maxcntx=nbcntx;
	this->nbworker=nbworker;

	long hostaddress;
	bzero(&(this->destination),sizeof(struct sockaddr_in));
	hostaddress = inet_addr(destination.c_str());
	if ( (long)hostaddress != (long)4294967295) {
		bcopy(&hostaddress,&(this->destination.sin_addr),sizeof(hostaddress));
	} else {
		struct hostent *serverent;
		serverent = gethostbyname(destination.c_str());
		bcopy(serverent->h_addr,&(this->destination.sin_addr),serverent->h_length);
	}
	this->destination.sin_port = htons(port);
	this->destination.sin_family = AF_INET;


	for(int i=0; i<this->nbworker;i++) {
		std::thread * tmp=new std::thread(ClientGenerator::run);
		this->workers->push_back(tmp);
	}

}


ClientGenerator * ClientGenerator::generator() {
	return ClientGenerator::singleton;
}

void ClientGenerator::run() {
	bool go=true;
	while (go) {
		long host=ClientGenerator::generator()->next();
		if (host<0) {
			go=false;
		} else {
			int socketfd;
			if ( (socketfd = ::socket(AF_INET,SOCK_STREAM,0)) >= 0) {
				if (host>0) {
					struct sockaddr_in source;
					bzero(&(source),sizeof(struct sockaddr_in));
					bcopy(&host,&(source).sin_addr,sizeof(host));
					source.sin_port = 0;
					source.sin_family = AF_INET;
					::bind(socketfd, (struct sockaddr *)&(source), sizeof(struct sockaddr_in));
				}
				if (::connect( socketfd,(struct sockaddr *)&(ClientGenerator::generator()->destination), sizeof(struct sockaddr_in)) >= 0 ) {
					Log::logger->log("ClientGenerator",DEBUG) << "Create Socket: " << socketfd << " thread id "<<  std::this_thread::get_id() <<endl;
					ConnectionManager::cm()->add(socketfd);
					::usleep(10000);
				}
			}
		}
	}
}

long ClientGenerator::next() {
	this->read.lock();
	if (this->remaininghost==0 && this->remainingcntx==0) {
		this->read.unlock();
		return -1;
	}
	if (this->remainingcntx==0) {
		this->remaininghost--;
		this->currenthost+=16777216;
		this->remainingcntx=this->maxcntx-1;
	} else {
		this->remainingcntx--;
	}
	this->read.unlock();
	return this->currenthost;
}

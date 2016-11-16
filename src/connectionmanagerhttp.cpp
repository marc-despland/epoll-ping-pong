#include "connectionmanagerhttp.h"
#include <netdb.h>
#include <fcntl.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include "httprequest.h"
#include "log.h"
#include <errno.h>

ConnectionManagerHttp::ConnectionManagerHttp(std::string host, int port, long max): ConnectionManager() {
	this->request=HttpRequest::request(host, port, "/");
	this->count=0;
	this->closed=false;
	this->max=max;
}

int ConnectionManagerHttp::add(int socket) {
	this->stoper.lock();
	this->sockets.push_back(socket);
	this->stoper.unlock();
	return ConnectionManager::add(socket);
}

int ConnectionManagerHttp::read(int socketfd) {
	Log::logger->log("ConnectionManagerHttp",DEBUG) << "Reading for socket " << socketfd <<endl;
    char buf[512];
    bzero(buf, 512);
    int count=0;
    int n=0;
    do {
    	n=::read (socketfd,  buf, sizeof buf);
    	if (n>0) count+=n;
    } while (n>0);
    Log::logger->log("ConnectionManagerHttp",DEBUG) << "Have read "<<count<< " bytes for socket " << socketfd << " Error : " << strerror(errno)<<endl;
    return count;
}



int ConnectionManagerHttp::write(int socketfd) {
	Log::logger->log("ConnectionManagerHttp",DEBUG) << "Writing for socket " << socketfd <<endl;
    this->count++;
	Log::logger->log("ConnectionManagerHttp",NOTICE) << "Writing request " << this->count <<endl;
	if (this->count>this->max) {
		this->stop();
		return -1;
	}
	return ::write(socketfd, this->request.c_str(), this->request.length());
}

void ConnectionManagerHttp::onStart() {
	this->starttime=time(NULL);
}

void ConnectionManagerHttp::stop() {
	this->stoper.lock();
	if (!this->closed) {
		this->closed=true;
		this->running=false;
		this->actions->unlock();
		for (int i=0; i<this->sockets.size();i++) ::close(this->sockets[i]);
	}
	this->stoper.unlock();
}

void ConnectionManagerHttp::showStats() {
	std::cout << "*******************************************************************************************" << endl;
	std::cout << "We have sent " << this->count << " requests in " << (time(NULL)-this->starttime) << " seconds" <<std::endl;
}

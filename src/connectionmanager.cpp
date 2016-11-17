#include "connectionmanager.h"
#include <netdb.h>
#include <fcntl.h>
#include <strings.h>
#include <unistd.h>
#include "log.h"
#include <errno.h>
#include <string.h>

#define MAXEVENTS 64

ConnectionManager::ConnectionManager() {
	this->events = (struct epoll_event *) calloc (MAXEVENTS, sizeof(struct epoll_event));
	this->pool = epoll_create1 (0);
	this->running=false;
	this->workers=new std::vector<std::thread *>();
	this->actions=new Fifo();
}



int ConnectionManager::add(int socket) {
	ConnectionManager::makeSocketNonBlocking(socket);
	struct epoll_event event;
	event.data.fd = socket;
  	event.events = EPOLLIN | EPOLLET;
  	int s = epoll_ctl (this->pool, EPOLL_CTL_ADD, socket, &event);
  	if (s<0) {
  		return -1;
  	} else {
  		return 1;
  	}

}

int ConnectionManager::makeSocketNonBlocking(int socket) {
	int flags, s;
	flags = fcntl (socket, F_GETFL, 0);
	if (flags == -1) {
    	return -1;
    }

	flags |= O_NONBLOCK;
	s = fcntl (socket, F_SETFL, flags);
	if (s == -1) {
      return -2;
    }
    return 1;
}


void ConnectionManager::onStart() {

}

void ConnectionManager::checkEvent() {
	this->onStart();
	this->running=true;
	while (this->running) {
		Log::logger->log("ConnectionManager",DEBUG) << "Checking events" <<endl;
		int n = epoll_wait (this->pool, this->events, MAXEVENTS, -1);
		Log::logger->log("ConnectionManager",DEBUG) << "Have received " <<n << "events"<<endl;
		for (int i = 0; i < n; i++) {
			if ((this->events[i].events & EPOLLERR) || (this->events[i].events & EPOLLHUP) || (!(this->events[i].events & EPOLLIN))) {
				Log::logger->log("ConnectionManager",ERROR) << "****************************************** CheckEvent ERROR on socket "<< this->events[i].data.fd <<endl;
			} else {
				Log::logger->log("ConnectionManager",DEBUG) << "Adding events"<<endl;
				this->actions->add(this->events[i].data.fd);
			}
		}
	}
}

int ConnectionManager::next() {
	Log::logger->log("ConnectionManager",DEBUG) << "Wait next socket"  <<endl;
	return this->actions->next();
}

void ConnectionManager::startWorkers(int count) {
	for(int i=0; i<count;i++) {
		std::thread * tmp=new std::thread(ConnectionManager::run, this);
		this->workers->push_back(tmp);
	}
}

void ConnectionManager::run(ConnectionManager * cm) {
	cm->running=true;
	while (cm->running) {
		Log::logger->log("ConnectionManager",DEBUG) << "Running worker"  <<endl;
		int socketfd=cm->next();
		if (socketfd>0) {
			Log::logger->log("ConnectionManager",DEBUG) << "Managing socket " <<socketfd <<endl;
			int count;
	        count = cm->read(socketfd);
	        if (count<0) {
	        	Log::logger->log("ConnectionManager",ERROR) << "*************************************************** An Error occurs reading socket => close "<<count<< " Error: "<< errno<< " " <<strerror(errno)<<endl;
	            ::close(socketfd);
	        } else {
				int nbw=cm->write(socketfd);
				if (nbw<=0) {
					Log::logger->log("ConnectionManager",ERROR) << "*************************************************** An Error occurs writing socket => close "<<nbw<< " Error: "<< errno<< " " <<strerror(errno)<<endl;
				}
			}
		}
	}
	Log::logger->log("ConnectionManager",NOTICE) << "Closing worker"  <<endl;

}

int ConnectionManager::read(int socketfd) {
	Log::logger->log("ConnectionManager",DEBUG) << "Reading on socket " << socketfd <<endl;
    char buf[512];
    bzero(buf, 512);
    return ::read (socketfd,  buf, sizeof buf);
}



int ConnectionManager::write(int socketfd) {
	Log::logger->log("ConnectionManager",DEBUG) << "Writing on socket " << socketfd <<endl;
	std:string bip="bip";
	return ::write(socketfd, bip.c_str(), bip.length());
}

void ConnectionManager::joinWorkers() {
	for (int i=0; i<this->workers->size(); i++ ) {
		this->workers->at(i)->join();
	}
}
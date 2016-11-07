#include "connectionmanager.h"
#include <netdb.h>
#include <fcntl.h>
#include <strings.h>
#include <unistd.h>

#define MAXEVENTS 64

ConnectionManager * ConnectionManager::singleton=new ConnectionManager();


ConnectionManager::ConnectionManager() {
	this->events = (struct epoll_event *) calloc (MAXEVENTS, sizeof(struct epoll_event));
	this->pool = epoll_create1 (0);
	this->running=false;
	this->workers=new std::vector<std::thread *>();
	this->actions=new Fifo();
}

ConnectionManager * ConnectionManager::cm() {
	return ConnectionManager::singleton;
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

void ConnectionManager::checkEvent() {
	this->running=true;
	while (this->running) {
		int n = epoll_wait (this->pool, this->events, MAXEVENTS, -1);
		for (int i = 0; i < n; i++) {
			if ((this->events[i].events & EPOLLERR) || (this->events[i].events & EPOLLHUP) || (!(this->events[i].events & EPOLLIN))) {
			//error
			} else {
				this->actions->add(this->events[i].data.fd);
			}
		}
	}
}

int ConnectionManager::next() {
	return this->actions->next();
}

void ConnectionManager::startWorkers(int count) {
	for(int i=0; i<count;i++) {
		std::thread * tmp=new std::thread(ConnectionManager::run);
		this->workers->push_back(tmp);
	}
}

void ConnectionManager::run() {
	ConnectionManager::cm()->running=true;
	while (ConnectionManager::cm()->running) {
		int socketfd=ConnectionManager::cm()->next();
		ssize_t count;
        char buf[512];
        bzero(buf, 512);
        count = read (socketfd,  buf, sizeof buf);
        if (count<=0) {
            ::close(socketfd);
        } else {
			::write(socketfd, buf, count);
		}
	}

}
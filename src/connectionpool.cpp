#include "connectionpool.h"
#include "log.h"

#define MAXEVENTS 64



ConnectionPool::ConnectionPool() throw (ConnectionPoolException) {
	this->events = (struct epoll_event *) calloc (MAXEVENTS, sizeof(struct epoll_event));
	this->pool = epoll_create1 (0);
	if (this->pool == -1) {
		throw ConnectionPoolException("Epoll create1 failed");
	}

}

void ConnectionPool::add(int socket) throw (ConnectionPoolException){
	//this->safe.lock();
	int pos=this->cntxArray.size();
	this->cntxArray.push_back(socket);
	this->cntxMap.insert(pair<int, int>(socket, pos));
	//this->safe.unlock();
	struct epoll_event event;
	event.data.fd = socket;
  	event.events = EPOLLIN | EPOLLET;
  	int s = epoll_ctl (this->pool, EPOLL_CTL_ADD, socket, &event);
  	if (s == -1) {
  		if (errno==ENOMEM) {
  			Log::logger->log("ConnectionPool", ERROR) << "No more memories in kernel" <<endl;
  		}
  		throw ConnectionPoolException("Epoll ctl failed");
    }

}


int ConnectionPool::next(int socket) {
	//this->safe.lock();
	unsigned int indice=this->cntxMap[socket];
	if ((indice+1)>=this->cntxArray.size()) indice=0;
	int result=this->cntxArray[indice+1];
	//this->safe.unlock();
	return result;
}
vector<int> * ConnectionPool::poll() {
	int n = epoll_wait (this->pool, this->events, MAXEVENTS, -1);
	vector<int> * result=new vector<int>();
	for (int i = 0; i < n; i++) {
		if ((this->events[i].events & EPOLLERR) || (this->events[i].events & EPOLLHUP) || (!(this->events[i].events & EPOLLIN))) {
			//error
		} else { 
			result->push_back(this->events[i].data.fd);
		}
	}
	return result;
}

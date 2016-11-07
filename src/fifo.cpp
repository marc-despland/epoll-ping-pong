#include "fifo.h"


Fifo::Fifo()  { 
}

void Fifo::add(int socket) {
	this->write.lock();
	this->fifo.push(socket);
	this->write.unlock();
	this->ready.notify_one();
}

int Fifo::next() {
	this->read.lock();
	std::unique_lock<std::mutex> lck(this->read);
	if (this->fifo.empty()) {
		this->ready.wait(lck);
	}
	int tmp=this->fifo.front();
	this->read.unlock();
	return tmp;
}

 
int Fifo::size() {
	int tmp=this->fifo.size();
	return tmp;
}

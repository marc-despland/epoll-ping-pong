#include "fifo.h"
#include "log.h"

Fifo::Fifo()  { 
	this->unlocked=false;
}

void Fifo::unlock() {
	this->unlocked=true;
	this->ready.notify_all();
}

void Fifo::add(int socket) {
	this->write.lock();
	this->fifo.push(socket);
	Log::logger->log("Fifo",DEBUG) << "Adding element" <<endl;
	this->write.unlock();
	this->ready.notify_one();
}

int Fifo::next() {
	Log::logger->log("Fifo",DEBUG) << "Next" <<endl;
	std::unique_lock<std::mutex> lck(this->read);
	Log::logger->log("Fifo",DEBUG) << "After unick lock" <<endl;
	if (this->fifo.empty()) {
		this->ready.wait(lck);
		if (this->unlocked) return -1;
	}
	int tmp=this->fifo.front();
	this->fifo.pop();
	return tmp;
}

 
int Fifo::size() {
	int tmp=this->fifo.size();
	return tmp;
}



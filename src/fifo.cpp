#include "fifo.h"


template <typename T> 
Fifo<T>::Fifo()  { 
}

template <typename T> 
void Fifo<T>::add(T object) {
	this->write.lock();
	this->fifo.push(object);
	this->write.unlock();
	this->ready.notify_one();
}

template <typename T> 
T Fifo<T>::next() {
	this->read.lock();
	if (this->fifo.empty()) {
		this->ready.wait(&(this->read));
	}
	T tmp=this->fifo.front();
	this->read.unlock();
	return tmp;
}

template <typename T> 
int Fifo<T>::size() {
	int tmp=this->fifo.size();
	return tmp;
}

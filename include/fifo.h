#ifndef _FIFO_H
#define _FIFO_H

#include <mutex>
#include <queue>
#include <condition_variable>
using namespace std;

template <typename T> 
class Fifo {
public:
	Fifo();
	void add(T object);
	T next();
	int size();

private:
	std::mutex read;
	std::mutex write;
	std::condition_variable ready;
	std::queue<T> fifo;

};

#endif
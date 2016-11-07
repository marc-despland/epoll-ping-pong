#ifndef _PINGPONGSERVER_H
#define _PINGPONGSERVER_H

#include <string>
using namespace std;
#include "runnable.h"
#include "connectionpool.h"


class MakeSocketNonBlockingException: public exception {
	public:
		MakeSocketNonBlockingException(string message):exception() {
			this->message=message;
		}
		~MakeSocketNonBlockingException() throw (){};
		friend std::ostream & operator<<(std::ostream &os, const MakeSocketNonBlockingException& e) {
			return os << e.message;
		}
	protected:
		string message;
};

class ConnectionListenException: public exception {
	public:
		ConnectionListenException(string message):exception() {
			this->message=message;
		}
		~ConnectionListenException() throw (){};
		friend std::ostream & operator<<(std::ostream &os, const ConnectionListenException& e) {
			return os << e.message;
		}
	protected:
		string message;
};
 
class ConnectionAcceptException: public exception {
	public:
		ConnectionAcceptException(string message):exception() {
			this->message=message;
		}
		~ConnectionAcceptException() throw (){};
		friend std::ostream & operator<<(std::ostream &os, const ConnectionAcceptException& e) {
			return os << e.message;
		}
	protected:
		string message;
};
class CantConnectException: public exception {
	public:
		CantConnectException(string message):exception() {
			this->message=message;
		}
		~CantConnectException() throw (){};
		friend std::ostream & operator<<(std::ostream &os, const CantConnectException& e) {
			return os << e.message;
		}
	protected:
		string message;
};



class PingPongServer:Runnable {

public:
	PingPongServer(int port, unsigned int size);
	void listen() throw (ConnectionListenException);
	void accept() throw (MakeSocketNonBlockingException);
	void run();

protected:
	static void makeSocketNonBlocking(int socket) throw(MakeSocketNonBlockingException);

	int socketfd;
	int port;
	unsigned int size;	
	ConnectionPool * pool;
	unsigned int count;	
	bool first;

};

#endif
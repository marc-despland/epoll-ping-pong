#ifndef _PINGPONGSERVER_H
#define _PINGPONGSERVER_H

#include <string>
using namespace std;
#include "connection.h"
#include "runnable.h"


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
	PingPongServer();
	void listen(unsigned int size) throw (ConnectionListenException);
	Connection * accept() throw (ConnectionAcceptException);
	void run();

protected:
	int port;
	int socketfd;
	Host * host;
	unsigned int size;		

};

#endif
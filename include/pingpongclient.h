#ifndef _PINGPONGCLIENT_H
#define _PINGPONGCLIENT_H

#include <string>
using namespace std;
#include "runnable.h"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

class ConnectionPoolException: public exception {
	public:
		ConnectionPoolException(string message):exception() {
			this->message=message;
		}
		~ConnectionPoolException() throw (){};
		friend std::ostream & operator<<(std::ostream &os, const ConnectionPoolException& e) {
			return os << e.message;
		}
	protected:
		string message;
};



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



class PingPongClient:Runnable {
public:
	PingPongClient(string host, int port, int size)throw(ConnectionPoolException);
	void connect() throw(CantConnectException);
	void createConnections();
	void run();
	void setSource(string host);
	void setSourceRange(string ip, int i);
protected:
	static void makeSocketNonBlocking(int socket) throw(MakeSocketNonBlockingException);
	string host;
	int port;
	int size;
	int		pool;
	struct epoll_event *events;
	int clientfd;
	struct sockaddr_in source;
	bool hasSource;

};
#endif
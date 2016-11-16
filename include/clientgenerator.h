#ifndef _CLIENTS_H
#define _CLIENTS_H
#include <thread>
#include <vector>
#include <mutex>
#include <string>
#include <netinet/in.h>
#include <sys/socket.h>
#include "connectionmanager.h"

class ClientGenerator {
public:
	ClientGenerator(ConnectionManager * cm);
	void start(std::string firsthost, int nbhost, int nbcntx, int nbworker, std::string destination, int port);
	void joinWorkers();
protected:
	virtual void dosomething(int socketfd);
	long next();
	static void run(ClientGenerator * generator);
	long currenthost;
	int  remaininghost;
	int remainingcntx;
	int maxcntx;
	int nbworker;
	std::vector<std::thread *> * workers;
	std::mutex read;
	struct sockaddr_in destination;
	ConnectionManager * cm;
};
#endif
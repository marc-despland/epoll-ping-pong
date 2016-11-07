#ifndef _CLIENTS_H
#define _CLIENTS_H
#include <thread>
#include <vector>
#include <mutex>
#include <string>
#include <netinet/in.h>
#include <sys/socket.h>

class ClientGenerator {
public:
	static ClientGenerator * generator();
	void start(std::string firsthost, int nbhost, int nbcntx, int nbworker, std::string destination, int port);

protected:
	long next();
	static ClientGenerator * singleton;
	ClientGenerator();
	static void run();
	long currenthost;
	int  remaininghost;
	int remainingcntx;
	int maxcntx;
	int nbworker;
	std::vector<std::thread *> * workers;
	std::mutex read;
	struct sockaddr_in destination;
};
#endif
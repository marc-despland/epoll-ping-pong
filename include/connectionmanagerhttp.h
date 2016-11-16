#ifndef _CONNECTIONMANAGERHTTP_H
#define _CONNECTIONMANAGERHTTP_H
#include "connectionmanager.h"
#include <string>
#include <ctime>
#include <mutex>


class ConnectionManagerHttp : public ConnectionManager {
public:
	ConnectionManagerHttp(std::string host, int port, long max);
	int add(int socket);
	void showStats();

protected:
	long max;
	int read(int socketfd);
	int write(int socketfd);
	void onStart();
	void stop();
	bool closed;

	std::vector<int> sockets;

	std::string request;
	time_t starttime;
	long count;
	std::mutex stoper;

};

#endif
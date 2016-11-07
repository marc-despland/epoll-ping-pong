#ifndef _CONNECTIONMANAGER_H
#define _CONNECTIONMANAGER_H
#include "fifo.h"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <vector>
#include <thread>

class ConnectionManager {
public:
	static ConnectionManager * cm();
	void 		checkEvent();
	int 		add(int socket);
	int			next();
	void		startWorkers(int count);
protected:
	ConnectionManager();
	static int makeSocketNonBlocking(int socket);
	static ConnectionManager * singleton;
	Fifo * actions;
	int		  pool;
	struct epoll_event *events;
	bool 	running;
	std::vector<std::thread *> * workers;
	static void run();
};
#endif
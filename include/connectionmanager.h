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
	ConnectionManager();
	virtual void checkEvent();
	virtual int 		add(int socket);
	int			next();
	void		startWorkers(int count);

	void joinWorkers();
protected:
	
	static int makeSocketNonBlocking(int socket);
	Fifo * actions;
	int		  pool;
	struct epoll_event *events;
	bool 	running;
	std::vector<std::thread *> * workers;
	static void run(ConnectionManager * cm);
	virtual void onStart();
	virtual int read(int socketfd);
	virtual int write(int socketfd);


};
#endif
#ifndef _CLIENTGENERATORHTTP_H
#define _CLIENTGENERATORHTTP_H
#include "clientgenerator.h"
#include <string>
#include "httprequest.h"

class ClientGeneratorHttp : public ClientGenerator {
public:
	ClientGeneratorHttp(ConnectionManager * cm);
	void start(std::string firsthost, int nbhost, int nbcntx, int nbworker, std::string destination, int port);
protected:
	virtual void dosomething(int socketfd);
	std::string request;

};

#endif
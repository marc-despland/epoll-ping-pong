#include "clientgeneratorhttp.h"
#include "log.h"
#include <unistd.h>


ClientGeneratorHttp::ClientGeneratorHttp(ConnectionManager * cm): ClientGenerator(cm) {

}

void ClientGeneratorHttp::dosomething(int socketfd) {
	//Log::logger->log("ClientGeneratorHttp",DEBUG) << "Do something: " << socketfd << " thread id "<<  std::this_thread::get_id() <<endl;
	int ws=write(socketfd, this->request.c_str(), this->request.length());
	if (ws<0) {
		Log::logger->log("ClientGeneratorHttp",ERROR) << "Can't write on socket: " << socketfd <<endl;
	}

}


void ClientGeneratorHttp::start(std::string firsthost, int nbhost, int nbcntx, int nbworker, std::string destination, int port) {
	this->request=HttpRequest::request(destination, port, "/");
	ClientGenerator::start(firsthost, nbhost, nbcntx, nbworker, destination, port);
}
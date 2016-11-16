#include <iostream>
#include "options.h"
#include "log.h"
#include "fifo.h"
#include "httpserver.h"


int main(int argc, char **argv) {
	Log::logger->setLevel(NOTICE);
	Options options(argv[0], "1.0.0", "Epoll Ping Pong https://github.com/marc-despland/epoll-ping-pong");
	try {
		options.add('d', "debug", "Start on debug mode", false, false);
		options.add('p', "port", "Port to listen to", true, true);
		options.add('w', "wait", "wait before closing", false, false);
		options.add('k', "keep-alive", "Keep socket alive", false, false);
	} catch(ExistingOptionException &e ) {
	}
	try {
		options.parse(argc, argv);
		if (options.get('d')->isAssign()) Log::logger->setLevel(DEBUG);
		HttpServer * server=new HttpServer(options.get("port")->asInt(),20, options.get('k')->isAssign());
		if (options.get('w')->isAssign()) server->waitBeforeClosing();
		server->run();
		
	} catch (OptionsStopException &e) {
	} catch (UnknownOptionException &e) {
		cout << " Request unknown option"<<endl;
	}

}
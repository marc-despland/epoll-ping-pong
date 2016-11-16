#include <iostream>
#include "options.h"
#include "log.h"
#include "fifo.h"
#include "connectionmanagerhttp.h"
#include "clientgeneratorhttp.h"


int main(int argc, char **argv) {
	Log::logger->setLevel(NOTICE);
	Options options(argv[0], "1.0.0", "Epoll Ping Pong https://github.com/marc-despland/epoll-ping-pong");
	try {
		options.add('d', "debug", "Start on debug mode", false, false);
		options.add('s', "host", "Host to connect to", true, true);
		options.add('p', "port", "Port to listen to", true, true);
		options.add('c', "count", "Number of connection to create", true, true);
		options.add('w', "workers", "Number of workers to create", true, true);
		options.add('f', "from", "The source IP to use", true, false);
		options.add('m', "max", "Number of kilo requests to send", true, true);
		options.add('n', "nbfrom", "The number of client (first start with 'from' ip... and increase)", true, false);
		} catch(ExistingOptionException &e ) {
	}
	try {
		options.parse(argc, argv);
		if (options.get('d')->isAssign()) Log::logger->setLevel(DEBUG);
		std::string from="0.0.0.0";
		int nbfrom=1;
		if (options.get("from")->isAssign()) {
			from=options.get("from")->asChars();
		}
		if (options.get("nbfrom")->isAssign()) nbfrom=options.get("nbfrom")->asInt();
		ConnectionManagerHttp * cm=new ConnectionManagerHttp( options.get("host")->asChars(),options.get("port")->asInt(),options.get("max")->asInt()*(long) 1000);
		ClientGeneratorHttp * generator=new ClientGeneratorHttp(cm);
		generator->start(from, nbfrom, options.get("count")->asInt(),options.get("workers")->asInt(), options.get("host")->asChars(),options.get("port")->asInt());
		Log::logger->log("KeepAlive",NOTICE) << "Waiting the end of client generation" <<endl;
		generator->joinWorkers();
		Log::logger->log("KeepAlive",NOTICE) << "End of client Generation" <<endl<<endl;
		cm->startWorkers(options.get("workers")->asInt());
		Log::logger->log("KeepAlive",NOTICE) << "Staring Event loops" <<endl;
		cm->checkEvent();
		
		//generator->joinWorkers();
		Log::logger->log("KeepAlive",NOTICE) << "Waiting the end of the workers" <<endl;
		cm->joinWorkers();
		cm->showStats();

	} catch (OptionsStopException &e) {
	} catch (UnknownOptionException &e) {
		cout << " Request unknown option"<<endl;
	}

}
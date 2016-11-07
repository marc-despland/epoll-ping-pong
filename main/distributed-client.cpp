#include <iostream>
#include "options.h"
#include "log.h"
#include "fifo.h"
#include "connectionmanager.h"
#include "clientgenerator.h"


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
		options.add('n', "nbfrom", "The number of client (first start with 'from' ip... and increase)", true, false);
		} catch(ExistingOptionException &e ) {
	}
	try {
		options.parse(argc, argv);
		if (options.get('d')->isAssign()) Log::logger->setLevel(DEBUG);
		if (options.get('n')->isAssign()) {
			int n=options.get("nb")->asInt();
		}
		std::string from="0.0.0.0";
		int nbfrom=1;
		if (options.get("from")->isAssign()) {
			from=options.get("from")->asChars();
		}
		if (options.get("nbfrom")->isAssign()) nbfrom=options.get("nbfrom")->asInt();
		ClientGenerator::generator()->start(from, nbfrom, options.get("count")->asInt(),options.get("workers")->asInt(), options.get("host")->asChars(),options.get("port")->asInt());
		ConnectionManager::cm()->startWorkers(options.get("workers")->asInt());
		ConnectionManager::cm()->checkEvent();


	} catch (OptionsStopException &e) {
	} catch (UnknownOptionException &e) {
		cout << " Request unknown option"<<endl;
	}

}
#include <iostream>
#include "options.h"
#include "log.h"
#include "fifo.h"
#include "pingpongclient.h"


int main(int argc, char **argv) {
	Log::logger->setLevel(NOTICE);
	Options options(argv[0], "1.0.0", "Epoll Ping Pong https://github.com/marc-despland/epoll-ping-pong");
	try {
		options.add('d', "debug", "Start on debug mode", false, false);
		options.add('s', "host", "Host to connect to", true, true);
		options.add('p', "port", "Port to listen to", true, true);
		options.add('c', "count", "Number of connection to create", true, true);
		options.add('f', "from", "The source IP to use", true, false);
		options.add('n', "nb", "The number of client (first start with 'from' ip... and increase)", true, false);
		} catch(ExistingOptionException &e ) {
	}
	try {
		options.parse(argc, argv);
		if (options.get('d')->isAssign()) Log::logger->setLevel(DEBUG);
		if (options.get('n')->isAssign()) {
			int count=options.get("count")->asInt();
			PingPongClient client[]=new PingPongClient[count];
			for (int i=0; i<n;i++) {
				client[i]=new PingPongClient(options.get("host")->asChars(), options.get("port")->asInt(),options.get("count")->asInt());
				client[i]->setSource(options.get("from")->asChars());
				client[i]->createConnections();
				

			}
		} else {
			PingPongClient * client=new PingPongClient(options.get("host")->asChars(), options.get("port")->asInt(),options.get("count")->asInt());
			if (options.get('f')->isAssign()) {
				client->setSource(options.get("from")->asChars());
			}
			client->createConnections();
			client->run();
		}
		
	} catch (OptionsStopException &e) {
	} catch (UnknownOptionException &e) {
		cout << " Request unknown option"<<endl;
	}

}
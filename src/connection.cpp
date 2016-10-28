#include "connection.h"


Connection::Connection(int socket,Host * host){
	this->socketfd=socket;
	this->host=host;
} 



Host * Connection::endpoint() {
	return this->host;
}

std::ostream & operator<<(std::ostream &os, const Connection * c) {
	return os << c->host;
}

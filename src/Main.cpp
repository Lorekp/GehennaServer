#include "Server.h"
#include "backend/DataHandler.h"

int main() {
	DataHandler DataHandler;
	Server server;
	server.run();
	return 0;
}
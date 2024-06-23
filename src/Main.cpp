#include "Server.h"
#include "backend/DataHandler.h"
#include "backend/PacketHandler.h"

int main() {
	DataHandler DataHandler();
	PacketHandler PacketHandler();
	Server server;
	server.run();
	return 0;
}
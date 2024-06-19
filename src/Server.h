#include "backend/DataHandler.h"
#include <SFML/Network.hpp>
#include <iostream>
#include <vector>
#include <filesystem>

class Server {
public:
	Server();
	int run() const;

private:
	const unsigned short PORT;
	int const SERVERCAPACITY;
};
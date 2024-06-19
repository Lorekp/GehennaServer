#include <SFML/Network.hpp>
#include <iostream>
#include <vector>

class Server {
public:
	Server();
	int run();

private:
	const unsigned short PORT;
};
#include "backend/DataHandler.h"
#include <SFML/Network.hpp>
#include <iostream>
#include <vector>
#include <filesystem>
#include <sqlite3.h>
#include <thread>
#include <mutex>

class Server {
public:
	Server();
	~Server();
	int run();


private:
	const unsigned short PORT;
	int const SERVERCAPACITY;
	std::map<std::string, sf::TcpSocket*> m_clients;

	sqlite3* playerDB;
	sqlite3* keyDB;

	std::thread* updateThread;
	sf::SocketSelector selector;

	std::mutex clientMutex;

	bool validateKey(const std::string& key, sqlite3* db);
	void authenticateClient(sf::TcpSocket* client, sqlite3* db);

	void update();
};
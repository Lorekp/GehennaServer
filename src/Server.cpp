#include "Server.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <map>
#include <string>
#include <SFML/Network.hpp>
#include <sqlite3.h>

namespace fs = std::filesystem;

Server::Server() : PORT(53000), SERVERCAPACITY(100) {
    fs::path data_dir = fs::current_path() / "data";
    fs::path db_path = data_dir / "key.db";

    // Open a database connection
    int exit = sqlite3_open(db_path.string().c_str(), &keyDB);

    if (exit != SQLITE_OK) {
        std::cerr << "Can't open key.db database: " << sqlite3_errmsg(keyDB) << std::endl;
    }
    else {
        std::cout << "Opened key.db database successfully" << std::endl;
    }

    updateThread = new std::thread(&Server::update, this);
}

Server::~Server() {
    updateThread->join();
}

int Server::run() {
    sf::TcpListener listener;

    if (listener.listen(PORT) != sf::Socket::Done) {
        std::cerr << "Error starting server on port " << PORT << std::endl;
        return -1;
    }

    selector.add(listener);
    std::cout << "Server is listening to port " << PORT << ", waiting for connections... " << std::endl;


    while (true) {
        std::cout << "Waiting for events..." << std::endl;
        if (selector.wait()) {
            std::cout << "Selector event detected!" << std::endl;
            if (selector.isReady(listener)) {
                sf::TcpSocket* client = new sf::TcpSocket;
                if (m_clients.size() < SERVERCAPACITY) {
                    if (listener.accept(*client) == sf::Socket::Done) {
                        authenticateClient(client, keyDB);
                    }
                    else {
                        std::cerr << "Failed to accept client connection." << std::endl;
                        delete client;
                    }
                }
                else {
                    delete client;
                }
            }
            else {
                for (auto it = m_clients.begin(); it != m_clients.end(); ) {
                    sf::TcpSocket& client = *(it->second);
                    if (selector.isReady(client)) {
                        sf::Packet packet;
                        if (client.receive(packet) == sf::Socket::Done) {
                            std::string message;
                            packet >> message;
                            std::cout << "Received message: " << message << std::endl;

                            // Broadcast the message to all clients
                            for (auto& pair : m_clients) {
                                if (pair.second != &client) {
                                    sf::Packet sendPacket;
                                    sendPacket << message;
                                    pair.second->send(sendPacket);
                                }
                            }
                            ++it;
                        }
                        else {
                            std::cout << "Client disconnected: " << client.getRemoteAddress() << std::endl;
                            selector.remove(client);
                            client.disconnect();
                            delete it->second;
                            it = m_clients.erase(it);
                        }
                    }
                    else {
                        ++it;
                    }
                }
            }
        }
    }
    sqlite3_close(keyDB);
    return 0;
}

// Function to validate client key
bool Server::validateKey(const std::string& key, sqlite3* db) {
    // Check database for key
    sqlite3_stmt* stmt;
    const char* query = "SELECT COUNT(*) FROM KEYS WHERE HASH = ? AND USED = 0";

    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_STATIC);

    int result = sqlite3_step(stmt);
    bool isValid = false;

    if (result == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        isValid = (count > 0);
    }

    sqlite3_finalize(stmt);

    // Check if key is currently in use
    if (m_clients.find(key) != m_clients.end()) {
        isValid = false;
    }

    return isValid;
}

void Server::authenticateClient(sf::TcpSocket* client, sqlite3* db) {
    sf::Packet packet;
    if (client->receive(packet) != sf::Socket::Done) {
        std::cerr << "Failed to receive packet from client." << std::endl;
        delete client;
        return;
    }

    std::string key;
    packet >> key;

    if (validateKey(key, db)) {
        std::cout << "Client authenticated with key: " << key << std::endl;
        {
            std::lock_guard<std::mutex> lock(clientMutex);
            m_clients.emplace(key, client);
            selector.add(*m_clients.find(key)->second);
        }

        sf::Packet responsePacket;
        responsePacket << "authenticated";
        client->send(responsePacket);

        std::cout << "New client connected: " << client->getRemoteAddress() << std::endl;
        
    }
    else {
        sf::Packet responsePacket;
        responsePacket << "denied";
        client->send(responsePacket);
        std::cout << "Client provided invalid key: " << key << std::endl;
        delete client;
    }
}

void Server::update() {
    while (true) {
        // Update stuff
        std::string message;
        std::getline(std::cin, message);

        if (message == "1") {
            int i = 1;
            for (auto it = m_clients.begin(); it != m_clients.end(); ++it) {
                std::cout << "Client " << i << ": " << it->second->getRemoteAddress() << std::endl;
                ++i;
            }
        }
        else if (message == "test")
        {
            for (auto& pair : m_clients) {
                sf::Packet sendPacket;
                sendPacket << message;
                pair.second->send(sendPacket);
            }
        }
    }
}

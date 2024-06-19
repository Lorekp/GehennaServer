#include "Server.h"

namespace fs = std::filesystem;

Server::Server() : PORT(53000), SERVERCAPACITY(100) {

}

int Server::run() const
{
    sf::TcpListener listener;
    std::vector<sf::TcpSocket*> clients;
    sf::SocketSelector selector;

    if (listener.listen(PORT) != sf::Socket::Done) {
        std::cerr << "Error starting server on port " << PORT << std::endl;
        return -1;
    }

    selector.add(listener);
    std::cout << "Server is listening to port " << PORT << ", waiting for connections... " << std::endl;

    while (true) {
        if (selector.wait()) {
            if (selector.isReady(listener)) {
                sf::TcpSocket* client = new sf::TcpSocket;
                if (clients.size() < SERVERCAPACITY)
                {
                    if (listener.accept(*client) == sf::Socket::Done) {
                        clients.push_back(client);
                        selector.add(*client);

                        std::cout << "New client connected: " << client->getRemoteAddress() << std::endl;
                    }
                    else {
                        delete client;
                    }
                }
                else {
                    delete client;
                }
            }
            else {
                for (auto it = clients.begin(); it != clients.end(); ++it) {
                    sf::TcpSocket& client = **it;
                    if (selector.isReady(client)) {
                        sf::Packet packet;
                        if (client.receive(packet) == sf::Socket::Done) {
                            std::string message;
                            packet >> message;
                            std::cout << "Received message: " << message << std::endl;

                            // Broadcast the message to all clients
                            for (auto& otherClient : clients) {
                                if (otherClient != &client) {
                                    sf::Packet sendPacket;
                                    sendPacket << message;
                                    otherClient->send(sendPacket);
                                }
                            }
                        }
                        else {
                            selector.remove(client);
                            client.disconnect();
                            delete& client;
                            clients.erase(it);
                            break;
                        }
                    }
                }
            }
        }
    }
	return 0;
}
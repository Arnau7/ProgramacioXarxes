#include <SFML\Network.hpp>
#include <SFML\Graphics.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

int main()
{
	std::cout << "Server online\n";
	std::vector<sf::TcpSocket*> aSocket;
	std::string textoAEnviar = "";
	sf::SocketSelector ss;
	sf::TcpSocket socket;
	int clientN;
	std::string mensaje;
	std::size_t bs;
	bool messageReceived = false;

	sf::TcpListener listener;
	sf::Socket::Status status = listener.listen(50000);
	if (status != sf::Socket::Status::Done) {
		std::cout << "End";
	}

	while (aSocket.size() < 4) {
		if (status == sf::Socket::Status::Done) {
			sf::TcpSocket* tempSocket = new sf::TcpSocket;
			sf::Socket::Status statusAccept = listener.accept(*tempSocket);
			if (statusAccept != sf::Socket::Status::Done) {
				std::cout << "Error accepting connection\n";
				delete tempSocket;
			}
			else {
				ss.add(*tempSocket);
				aSocket.push_back(tempSocket);
				std::cout << "New client: " << tempSocket->getRemoteAddress().toString() << "\n";
				mensaje = "New client online";
				for (int i = 0; i < aSocket.size() - 1; i++) {
					aSocket[i]->send(mensaje.c_str(), mensaje.length());
				}
			}
		}
	}
	std::cout << "\n4 Clients online\n";
	listener.close();

	mensaje = "All players connected\n";
	for (int i = 0; i < 4; i++) {
		aSocket[i]->send(mensaje.c_str(), mensaje.length());
	}

	while (true) {
		if (ss.wait()) {
			for (int i = 0; i < aSocket.size(); i++)
			{
				if (ss.isReady(*aSocket[i]))
				{
					char buffer[2000];
					size_t bytesReceived;

					sf::Socket::Status statusReceive = aSocket[i]->receive(buffer, 2000, bytesReceived);
					if (statusReceive == sf::Socket::NotReady) {
						break;
					}
					else if (statusReceive == sf::Socket::Done) {
						std::cout << "He rebut correctament\n";
						buffer[bytesReceived] = '\0';
						mensaje = buffer;
						clientN = i;
						messageReceived = true;
					}
					else if (statusReceive == sf::Socket::Disconnected) {
						ss.remove(*aSocket[i]);
						//aSocket.erase[&i];
						mensaje = "S'ha desconectat el client: " + std::to_string(i);
						aSocket.erase(aSocket.begin() + (i));
						for (int j = 0; j < aSocket.size(); j++) {
							aSocket[j]->send(mensaje.c_str(), mensaje.length());
						}
						break;
					}
					break;
				}
			}
			if (messageReceived) {
				for (int j = 0; j < aSocket.size(); j++) {
					if (j != clientN)
					{
						std::cout << " L'envii al client " << j << "\n";
						sf::Socket::Status statusSend = aSocket[j]->send(mensaje.c_str(), mensaje.length(), bs);
						while (statusSend == sf::Socket::Status::Partial)
						{
							mensaje = mensaje.substr(bs + 1, bs);
							statusSend = aSocket[j]->send(mensaje.c_str(), mensaje.length(), bs);
						}
						if (statusSend == sf::Socket::Status::Done)
							std::cout << "L'he enviat correctament \n";
					}
				}
				messageReceived = false;
				mensaje = "";
			}
		}
	}
}
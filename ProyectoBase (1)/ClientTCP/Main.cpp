#include <SFML\Network.hpp>
#include <SFML\Graphics.hpp>
#include <string>
#include <iostream>
#include <mutex>


int main() {
	std::cout << "Client online\n";

	sf::TcpSocket socket;
	socket.setBlocking(false);
	sf::Socket::Status socketStatus = socket.connect("localhost", 50000, sf::milliseconds(15.f));
	if (socketStatus == sf::Socket::Status::Done) {
		std::cout << "Connected to server\n";
	}

	std::mutex mu;

	//Xat
	std::vector<std::string> aMensajes;

	sf::Vector2i screenDimensions(800, 600);

	sf::RenderWindow window;
	window.create(sf::VideoMode(screenDimensions.x, screenDimensions.y), "Chat");

	sf::Font font;
	if (!font.loadFromFile("arial.ttf"))
	{
		std::cout << "Can't load the font file" << std::endl;
	}

	std::string mensaje = " >";

	sf::Text chattingText(mensaje, font, 14);
	chattingText.setFillColor(sf::Color(0, 160, 0));
	chattingText.setStyle(sf::Text::Bold);


	sf::Text text(mensaje, font, 14);
	text.setFillColor(sf::Color(0, 160, 0));
	text.setStyle(sf::Text::Bold);
	text.setPosition(0, 560);

	sf::RectangleShape separator(sf::Vector2f(800, 5));
	separator.setFillColor(sf::Color(200, 200, 200, 255));
	separator.setPosition(0, 550);

	while (window.isOpen())
	{
		sf::Event evento;
		while (window.pollEvent(evento))
		{
			switch (evento.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				if (evento.key.code == sf::Keyboard::Escape)
					window.close();
				else if (evento.key.code == sf::Keyboard::Return)
				{
					aMensajes.push_back(mensaje);
					if (aMensajes.size() > 25)
					{
						aMensajes.erase(aMensajes.begin(), aMensajes.begin() + 1);
					}
					if (mensaje == ">exit" || mensaje == "exit")
					{
						std::size_t bs;
						mensaje = "Chat ended";
						std::cout << mensaje << "\n";
						sf::Socket::Status statusSend = socket.send(mensaje.c_str(), mensaje.length(), bs);
						while (statusSend == sf::Socket::Status::Partial)
						{
							mensaje = mensaje.substr(bs + 1, bs);
							statusSend = socket.send(mensaje.c_str(), mensaje.length(), bs);
						}
						mensaje = ">";
						window.display();
						window.clear();
						window.close();
						socket.disconnect();
					}

					else {//SEND
						std::size_t bs;
						std::cout << mensaje << "\n";
						sf::Socket::Status statusSend = socket.send(mensaje.c_str(), mensaje.length(), bs);
						while (statusSend == sf::Socket::Status::Partial)
						{
							mensaje = mensaje.substr(bs + 1, bs);
							statusSend = socket.send(mensaje.c_str(), mensaje.length(), bs);
						}
						mensaje = ">";
					}
				}
				break;
			case sf::Event::TextEntered:
				if (evento.text.unicode >= 32 && evento.text.unicode <= 126)
					mensaje += (char)evento.text.unicode;
				else if (evento.text.unicode == 8 && mensaje.length() > 0)
					mensaje.erase(mensaje.length() - 1, mensaje.length());
				break;
			}

		}
		while (true) {

			char buffer[2000];
			size_t bytesReceived;

			sf::Socket::Status statusReceive = socket.receive(buffer, 2000, bytesReceived);
			if (statusReceive == sf::Socket::NotReady) {
				break;
			}
			else if (statusReceive == sf::Socket::Done) {
				buffer[bytesReceived] = '\0';
				aMensajes.push_back(buffer);
			}
			else if (statusReceive == sf::Socket::Disconnected) {
				return 0;
			}
		}

		window.draw(separator);
		for (size_t i = 0; i < aMensajes.size(); i++)
		{
			std::string chatting = aMensajes[i];
			chattingText.setPosition(sf::Vector2f(0, 20 * i));
			chattingText.setString(chatting);
			window.draw(chattingText);
		}
		std::string mensaje_ = mensaje + "_";
		text.setString(mensaje_);
		window.draw(text);


		window.display();
		window.clear();
	}
	socket.disconnect();	
}
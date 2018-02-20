#include <SFML\Network.hpp>
#include <SFML\Graphics.hpp>
#include <string>
#include <iostream>

int main()
{

	std::cout << "Ets el servidor";
	
	sf::TcpSocket socket;
	std::string textoAEnviar = "";
	std::vector<sf::TcpSocket*> aSocket;
	sf::SocketSelector ss;

		
	sf::TcpListener listener;
	while (aSocket.size() < 4) {
		sf::Socket::Status status = listener.listen(50000);
		if (status == sf::Socket::Status::Done) {
			sf::TcpSocket* tempSocket = new sf::TcpSocket;
			sf::Socket::Status statusAccept = listener.accept(*tempSocket);
			ss.add(*tempSocket);
			aSocket.push_back(tempSocket);
			if (statusAccept != sf::Socket::Status::Done) {
				std::cout << "Error al acceptar connexió\n";
			}
		}
	}
	listener.close();
	/*
	std::string texto = "Conexion con ... " + (socket->getRemoteAddress()).toString() + ":" + std::to_string(socket->getRemotePort()) + "\n";
	std::cout << texto;

	std::vector<std::string> aMensajes;

		MyFunctorSS ss_functor(&aMensajes, &ss, aSocket);
		std::thread t(ss_functor);

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
							mensaje = "Chat finalizado";
							std::cout << mensaje << "\n";
							sf::Socket::Status status = socket->send(mensaje.c_str(), mensaje.length());
							if (status != sf::Socket::Status::Done) {
								std::cout << "ERROR";
							}
							mensaje = ">";
							window.display();
							window.clear();
							window.close();
							socket->disconnect();
						}
						else {
							//SEND
							std::cout << mensaje << "\n";
							sf::Socket::Status status = socket->send(mensaje.c_str(), mensaje.length());
							if (status != sf::Socket::Status::Done) {
								std::cout << "ERROR";
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
		socket->disconnect();*/
		return 0;
}
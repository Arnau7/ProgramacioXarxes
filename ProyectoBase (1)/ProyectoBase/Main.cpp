#include <SFML\Network.hpp>
#include <SFML\Graphics.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#define MAX_MENSAJES 30
std::mutex mu;

class MyFunctor
{
private:
	sf::TcpSocket*socket;
	std::vector<std::string>*aMensajes;
public:
	MyFunctor(sf::TcpSocket*socket, std::vector<std::string>*aMensajes) {
		this->socket = socket;
		this->aMensajes = aMensajes;
	}
	void operator()() {
		while (true) {
			char buffer[2000];
			size_t bytesReceived;
			
			sf::Socket::Status status = socket->receive(buffer, 2000, bytesReceived);
			if (status == sf::Socket::Status::Disconnected) {
				break;
			}
			buffer[bytesReceived] = '\0';
			mu.lock();
			this->aMensajes->push_back(buffer);
			mu.unlock();
			
		}
	}

};

class MyFunctorSS
{
private:
	sf::SocketSelector* ss;
	std::vector<std::string>*aMensajes;
	std::vector<sf::TcpSocket*>aSocket;

public:
	MyFunctorSS(std::vector<std::string>*aMensajes, sf::SocketSelector* ss, std::vector<sf::TcpSocket*> aSocket) {
		this->ss = ss;
		this->aMensajes = aMensajes;
		this->aSocket = aSocket;
	}
	void operator()() {
		while (true) {
			if (ss->wait()) {
				//std::cout << "Tenc una peticio\n";
				for (int i = 0; i < aSocket.size(); i++)
				{
					if (ss->isReady(*aSocket[i]))
					{
						//std::cout << "He trobat el socket, es el " << i << "\n";
						char buffer[2000];
						size_t bytesReceived;

						sf::Socket::Status statusReceive = aSocket[i]->receive(buffer, 2000, bytesReceived);
						if (statusReceive == sf::Socket::NotReady) {
							break;
						}
						else if (statusReceive == sf::Socket::Done) {
							buffer[bytesReceived] = '\0';
							aMensajes->push_back(buffer);
						}
						else if (statusReceive == sf::Socket::Disconnected) {
							break;
						}
						break;
					}

				}

			}
		}
	}

};

int BlockingThreading(sf::TcpSocket* socket) 
{
	std::cout << "Blocking chat \n";

	std::string texto = "Conexion con ... " + (socket->getRemoteAddress()).toString() + ":" + std::to_string(socket->getRemotePort()) + "\n";
	std::cout << texto;

	std::vector<std::string> aMensajes;

	//RECEIVE
	MyFunctor fnctor(socket, &aMensajes);
	std::thread t(fnctor);

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
		//RECEIVE -- thread
		//fnctor.Reception(&socket, &aMensajes);

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
	socket->disconnect();
}

int NonBlocking(sf::TcpSocket* socket) 
{
	
	std::cout << "NonBlocking chat \n";

	socket->setBlocking(false);

	std::string texto = "Conexion con ... " + (socket->getRemoteAddress()).toString() + ":" + std::to_string(socket->getRemotePort()) + "\n";
	std::cout << texto;

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
						mensaje = "Chat finalizado";
						std::cout << mensaje << "\n";
						sf::Socket::Status statusSend = socket->send(mensaje.c_str(), mensaje.length(), bs);
						while (statusSend == sf::Socket::Status::Partial)
						{
							mensaje = mensaje.substr(bs + 1, bs);
							statusSend = socket->send(mensaje.c_str(), mensaje.length(), bs);
						}
						mensaje = ">";
						window.display();
						window.clear();
						window.close();
						socket->disconnect();
					}

					else {//SEND
						std::size_t bs;
						std::cout << mensaje << "\n";
						sf::Socket::Status statusSend = socket->send(mensaje.c_str(), mensaje.length(), bs);
						while (statusSend == sf::Socket::Status::Partial)
						{
							mensaje = mensaje.substr(bs + 1, bs);
							statusSend = socket->send(mensaje.c_str(), mensaje.length(), bs);
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
		//RECEIVE -- thread
		//fnctor.Reception(&socket, &aMensajes);
		while (true) {

			char buffer[2000];
			size_t bytesReceived;

			sf::Socket::Status statusReceive = socket->receive(buffer, 2000, bytesReceived);
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
	socket->disconnect();
}

int SocketSelector(sf::TcpSocket* socket) 
{
	std::cout << "Socket selector chat\n";

	sf::SocketSelector ss;
	ss.add(*socket);
	
	std::vector<sf::TcpSocket*> aSocket;
	aSocket.push_back(socket);


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
	socket->disconnect();
}

int main()
{

	std::cout << "¿Seras servidor (s) o cliente (c)? ... ";
	char c;
	std::cin >> c;
	sf::TcpSocket socket;
	std::string textoAEnviar = "";

	char modeBuffer[1];
	// Hola
	if (c == 's')
	{
		std::cout << "_____ Elige un modo _____ \n" << "1- Blocking & Threading \n" << "2- NonBlocking \n" << "3- SocketSelector\n";
		std::string mode;
		std::cin >> mode;

		sf::TcpListener listener;
		listener.listen(50000);
		listener.accept(socket);
		socket.send(mode.c_str(), mode.length());

		if (mode == "1") {
			BlockingThreading(&socket);
		}
		else if (mode == "2") {
			NonBlocking(&socket);
		}
		else if (mode == "3") {
			SocketSelector(&socket);
		}
		else
		{
			std::cout << "Invalid number";
		}
	}
	else if (c == 'c')
	{
		socket.connect("localhost", 50000, sf::milliseconds(15.f));

		// Rebre el mode seleccionat per el servidor
		size_t bytesReceived;
		socket.receive(modeBuffer, 1, bytesReceived);
		//std::cout << modeBuffer;

		if (modeBuffer[0] == '1') {
			BlockingThreading(&socket);
		}
		else if (modeBuffer[0] == '2') {
			NonBlocking(&socket);
		}
		else if (modeBuffer[0] == '3') {
			SocketSelector(&socket);
		}
	}
	else
	{
		exit(0);
	}

}

/*
int main()
{
std::cout << "¿Seras servidor (s) o cliente (c)? ... ";
char c;
std::cin >> c;
sf::TcpSocket socket;
std::string textoAEnviar="";
if (c == 's')
{
sf::TcpListener listener;
listener.listen(50000);
listener.accept(socket);
textoAEnviar = "Mensaje desde servidor\n";
}
else if (c == 'c')
{
socket.connect("localhost", 50000, sf::milliseconds(15.f));
textoAEnviar = "Mensaje desde cliente\n";

}
else
{
exit(0);
}
std::string texto = "Conexion con ... " + (socket.getRemoteAddress()).toString() + ":" + std::to_string(socket.getRemotePort()) + "\n";
std::cout << texto;

socket.send(textoAEnviar.c_str(), texto.length());

char buffer[100];
size_t bytesReceived;
socket.receive(buffer, 100, bytesReceived);

buffer[bytesReceived] = '\0';
std::cout << "Mensaje recibido: " << buffer << std::endl;



system("pause");

return 0;

}*/

/*
void Reception(sf::TcpSocket*socket, std::vector<std::string>*aMensajes) {
char buffer[100];
size_t bytesReceived;
//sf::Socket::Status status = socket.receive;
int a = 1;
while (a != 0) {
socket->receive(buffer, 100, bytesReceived);
aMensajes->push_back(buffer);
if (aMensajes->size() > 25)
{
aMensajes->erase(aMensajes->begin(), aMensajes->begin() + 1);
}
}
}*/

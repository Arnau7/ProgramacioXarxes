#include <SFML\Network.hpp>
#include <SFML\Graphics.hpp>
#include <string>
#include <iostream>
#include <vector>

using namespace std;
using namespace sf;

struct direction
{
	string ip;
	unsigned short port;
	direction(string _ip, unsigned short _port) : ip(_ip), port(_port){}
};

vector<direction> aPeers;

void main()
{
	TcpListener listener;
	listener.listen(50000);
	for (int i = 0; i < 4; i++) 
	{
		TcpSocket sock;
		Socket::Status statusAccept = listener.accept(sock);
		if (statusAccept == Socket::Status::Done) 
		{
			direction d(sock.getRemoteAddress().toString(), sock.getRemotePort());
			
			//Construir 1 mensaje
			Packet packet;
			packet << d.ip << d.port;
			sock.send(packet);
			sock.disconnect();
			cout << "HELLO PEER\n";
			aPeers.push_back(d);
		}
	}
	listener.close();
}



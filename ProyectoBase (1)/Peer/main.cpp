#include <SFML\Network.hpp>
#include <SFML\Graphics.hpp>
#include <string>
#include <iostream>
#include <vector>

using namespace std;
using namespace sf;

vector<TcpSocket*>aSock;

struct direction
{
	string ip;
	unsigned short port;
	direction(string _ip, unsigned short _port) : ip(_ip), port(_port) {}
};

void main() 
{
	TcpSocket sock;
	sock.connect("localhost", 50000);
	Packet packet;
	Socket::Status statusReceive = sock.receive(packet);
	if (statusReceive == Socket::Status::Done) 
	{
		cout << "RECEIVED\n";
	}
	vector<direction>aPeersStr;
	string ip;
	unsigned short port;
	packet >> ip >> port;
	direction d(ip, port);
	aPeersStr.push_back(d);
	cout << "HELLO SERVER\n" << ip << endl << port << endl;
	for (int i = 0; i < aPeersStr.size(); i++)
	{
		TcpSocket sock;
		sock.connect(aPeersStr[i].ip, aPeersStr[i].port);
		aSock.push_back(&sock);
	}

	int port2 = sock.getLocalPort();
	sock.disconnect();
	TcpListener listener;
	listener.listen(port2);

	for (int i = aSock.size(); i < aSock.size(); i++) 
	{
		TcpSocket sockAux;
		listener.accept(sockAux);
		aSock.push_back(&sockAux);
	}
	listener.close();
	cout << aSock.size();
	getchar();

	//JUGAR

}
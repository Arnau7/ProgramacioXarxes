#include <SFML\Network.hpp>
#include <SFML\Graphics.hpp>
#include <string>
#include <iostream>
#include <vector>

using namespace std;
using namespace sf;

struct Direction
{
	string ip;
	unsigned short port;
	Direction(string _ip, unsigned short _port) : ip(_ip), port(_port){}
	Direction(){}
};

vector<Direction> aPeers;
void main()
{
	int maxPeers = 4;
	int conectedPeers = 0;
	TcpListener listener;
	listener.listen(50000);
	for (int i = 0; i < maxPeers; i++)
	{
		TcpSocket sock;
		Socket::Status statusAccept = listener.accept(sock);
		string str = to_string(maxPeers) + '+' + to_string(conectedPeers) + '-';
		if (statusAccept == Socket::Status::Done)
		{
			for (int i = 0; i < conectedPeers; i++) {
				str += aPeers[i].ip + '_' + to_string(aPeers[i].port) + 'H';
				cout << "Client afegit a l'string \n";
			}

			Direction d(sock.getRemoteAddress().toString(), sock.getRemotePort());
			Packet packet;
			packet << str;
			sock.send(packet);
			cout << str << "\n";
			sock.disconnect();
			aPeers.push_back(d);
			conectedPeers++;
		}
	}
	listener.close();
}



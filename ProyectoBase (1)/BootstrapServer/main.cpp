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
	int maxPeers = 4;
	int conectedPeers = 0;
	Packet packet;
	TcpListener listener;
	listener.listen(50000);
	for (int i = 0; i < maxPeers; i++) 
	{
		string str = to_string(maxPeers) + '+' + to_string(conectedPeers) + '-';
		TcpSocket sock;
		Socket::Status statusAccept = listener.accept(sock);
		if (statusAccept == Socket::Status::Done) 
		{
			for (int i = 0; i < conectedPeers; i++) {
				str += aPeers[i].ip + '.' + to_string(aPeers[i].port) + '#';
			}
			direction d(sock.getRemoteAddress().toString(), sock.getRemotePort());
			packet << d.ip << d.port;
			cout << "Direction added to the package, it's length now is: " << packet.getDataSize() << "\n";
			sock.send(packet);
			sock.disconnect();
			aPeers.push_back(d);
		}
	}
	listener.close();
}



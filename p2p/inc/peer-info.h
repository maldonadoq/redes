#ifndef _PEER_INFO_H_
#define _PEER_INFO_H_

#include <string>

using std::cout;
using std::string;
using std::to_string;

struct TPeerInfo
{
	std::string m_ip;
	int m_port;
	bool m_keep = true;
};

string PeerToStr(TPeerInfo _peer){
	return _peer.m_ip + "|" + to_string(_peer.m_port);
}

void PrintPeer(TPeerInfo _peer){
	cout << "ip: " + _peer.m_ip + "\tport: " + to_string(_peer.m_port);
}

#endif
#ifndef _PEER_INFO_H_
#define _PEER_INFO_H_

#include <string>

using std::string;
using std::to_string;

struct TPeerInfo
{
	std::string m_ip;
	int m_port;
};

string PeerToStr(TPeerInfo _peer){
	return _peer.m_ip + "|" + to_string(_peer.m_port);
}

#endif
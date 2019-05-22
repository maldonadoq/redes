#ifndef _SOCKET_INFO_H_
#define _SOCKET_INFO_H_

#include <string>

class TSocket{
public:
	std::string m_ip;
	bool m_state;
	int m_sock;

	TSocket(){
		this->m_state = true;
	}

	~TSocket(){	};

	void SetIp(std::string);
};

void TSocket::SetIp(std::string _ip){
	this->m_ip = _ip;
}

#endif
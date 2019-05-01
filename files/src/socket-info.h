#ifndef _SOCKET_INFO_H_
#define _SOCKET_INFO_H_

#include <string>

class TSocket{
public:
	int m_id;
	int m_sock;

	TSocket(){	};
	~TSocket(){	};

	void SetId(int);
};

void TSocket::SetId(int _id){
	this->m_id = _id;
}

#endif
#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <thread>
#include "read-write.h"

extern unsigned rsize;
extern unsigned csize;

class TClient{
private:
	int m_clientSock;
	struct sockaddr_in m_clientAddr;
public:
	TClient();
	void Connect(std::string, int);
	void Talking(int);
};

TClient::TClient(){
	this->m_clientSock = socket(AF_INET, SOCK_STREAM, 0);
}

void TClient::Connect(std::string _ip, int _port){
    if(m_clientSock < 0){
        perror("Can not Create Socket!");
        exit(0);
    }

    memset(&m_clientAddr, 0, sizeof(m_clientAddr));
    m_clientAddr.sin_family = AF_INET;
    m_clientAddr.sin_port = htons(_port);
    // m_clientAddr.sin_addr.s_addr = inet_addr(_ip.c_str());
 
    int res = inet_pton(AF_INET, _ip.c_str(), &m_clientAddr.sin_addr);
 
    if (0 > res){
      perror("error: first parameter is not a valid address family");
      close(m_clientSock);
      exit(EXIT_FAILURE);
    }
    else if (0 == res){
      perror("char string (second parameter does not contain valid ipaddress");
      close(m_clientSock);
      exit(EXIT_FAILURE);
    }

    if (connect(m_clientSock, (const struct sockaddr *)&m_clientAddr, sizeof(m_clientAddr)) < 0){
        perror("Connect failed"); 
        exit(1);
    }

    std::cout << "Client Connected\n";
}

void TClient::Talking(int _speed){
    std::cout << "Talking\n";

    std::string text;
    bool t = true;

    unsigned buffer_size = 2;
	char buffer[buffer_size];

	while(t){
		getline(std::cin, text);
		send(m_clientSock, text.c_str(), text.size(), 0);
		
		if(recv(m_clientSock, buffer, buffer_size, 0) > 0){
			if(buffer[0] == '1'){
				std::cout << "Connected\n";
				t = false;
			}
			else if(buffer[0] == '0'){
				std::cout << "This avatar already exist!\n";
			}
		}
	}

    std::thread tread(thread_read,   m_clientSock, rsize, csize);
    std::thread twrite(thread_write, m_clientSock);
    std::thread twrites(thread_write_speed, m_clientSock, _speed);

    tread.join();
    // std::cout << "holaa\n";
    // twrite.kill();
    twrite.join();
    twrites.join();

    close(m_clientSock);
}

#endif
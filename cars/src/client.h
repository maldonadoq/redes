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
    int m_client_sock;
    struct sockaddr_in m_client_addr;
public:
    TClient();
    void Connect(std::string, int);
    void Talking();
};

TClient::TClient(){
    this->m_client_sock = socket(AF_INET, SOCK_STREAM, 0);
}

void TClient::Connect(std::string _ip, int _port){
    if(m_client_sock < 0){
        perror("Can not Create Socket!");
        exit(0);
    }

    memset(&m_client_addr, 0, sizeof(m_client_addr));
    m_client_addr.sin_family = AF_INET;
    m_client_addr.sin_port = htons(_port);
 
    int res = inet_pton(AF_INET, _ip.c_str(), &m_client_addr.sin_addr);
 
    if (0 > res){
      perror("error: first parameter is not a valid address family");
      close(m_client_sock);
      exit(EXIT_FAILURE);
    }
    else if (0 == res){
      perror("char string (second parameter does not contain valid ipaddress");
      close(m_client_sock);
      exit(EXIT_FAILURE);
    }

    if (connect(m_client_sock, (const struct sockaddr *)&m_client_addr, sizeof(m_client_addr)) < 0){
        perror("Connect failed"); 
        exit(1);
    }

    std::cout << "Client Connected\n";
}

void TClient::Talking(){
    std::cout << "Client Talking\n";

    std::thread tread(thread_read,   m_client_sock, rsize, csize);
    std::thread twrite(thread_write, m_client_sock);

    tread.join();
    // std::terminate();
    twrite.join();

    close(m_client_sock);
}

#endif
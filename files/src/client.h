#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <mutex>
#include <vector>

#include "socket-info.h"
#include "thread-u.h"

#include "read-write.h"

int Connect = true;

class TClient{
private:
    static int m_size;
    static int m_clientSockC;
    static int m_clientSockS;

    struct sockaddr_in m_clientAddrC;
    struct sockaddr_in m_clientAddrS;

    static int unique_port;

    static std::mutex m_qmutex;
public:
    TClient(int);

    void ConnectToServer(std::string, int);
    static void TalkingToServer(int);

    void CreateServer(int);
    static void ListeningServer();
    static void HandleServerClient(TSocket *);
};

std::mutex TClient::m_qmutex;

int TClient::unique_port;
int TClient::m_size;
int TClient::m_clientSockC;
int TClient::m_clientSockS;

TClient::TClient(int _size){
    this->m_size = _size;
    this->m_clientSockC = socket(AF_INET, SOCK_STREAM, 0);
    this->m_clientSockS = socket(AF_INET, SOCK_STREAM, 0);
}

void TClient::ConnectToServer(std::string _ip, int _port){
    if(m_clientSockC < 0){
        perror("Can not Create Socket!");
        exit(0);
    }

    memset(&m_clientAddrC, 0, sizeof(m_clientAddrC));
    m_clientAddrC.sin_family = AF_INET;
    m_clientAddrC.sin_port = htons(_port);
 
    int res = inet_pton(AF_INET, _ip.c_str(), &m_clientAddrC.sin_addr);
 
    if (0 > res){
      perror("error: first parametm_clientSockCer is not a valid address family");
      close(m_clientSockC);
      exit(EXIT_FAILURE);
    }
    else if (0 == res){
      perror("char string (second parameter does not contain valid ipaddress");
      close(m_clientSockC);
      exit(EXIT_FAILURE);
    }

    if (connect(m_clientSockC, (const struct sockaddr *)&m_clientAddrC, sizeof(m_clientAddrC)) < 0){
        perror("Connect failed"); 
        exit(1);
    }
}

void TClient::TalkingToServer(int _loop){
    std::cout << "Client Listenig-Talking\n";
    std::thread twrite(thread_write, m_clientSockC, _loop, m_size);
    twrite.join();

    Connect = false;
    close(m_clientSockC);    
    std::cout << "client finished talking!\n";
}


void TClient::CreateServer(int _port){
    this->unique_port = _port;
    int reuse = 1;  
    memset(&m_clientAddrS, 0, sizeof(sockaddr_in));

    m_clientAddrS.sin_family = AF_INET;
    m_clientAddrS.sin_addr.s_addr = INADDR_ANY;
    m_clientAddrS.sin_port = htons(_port);

    //Avoid bind error if the socket was not close()
    setsockopt(m_clientSockS,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));

    if(bind(m_clientSockS, (struct sockaddr *) &m_clientAddrS, sizeof(sockaddr_in)) < 0)
        perror("Failed to bind");

    listen(m_clientSockS, 5);
    std::cout << "Server to Clients Created! [Listening]\n";
}

void TClient::ListeningServer(){
    int buffer_size = m_size;
    char buffer[buffer_size];
    int n;
    std::string tmp;

    int ConnectFD = accept(m_clientSockS, NULL, NULL);
    if(0 > ConnectFD){
        perror("Error accept failed");
        close(m_clientSockS);
        exit(0);
    }

    std::ofstream fs(std::to_string(unique_port)+".txt"); 
    while(Connect){ 
        bzero(buffer,buffer_size);
        n = recv(ConnectFD, buffer, buffer_size, 0);

        if(n > 0){
            tmp = read_buffer(buffer, m_size);
            printf("%s\n", tmp.c_str());
            fs << tmp << "\n";
        }
    }

    fs.close();

    close(ConnectFD);
    close(m_clientSockS);
}

#endif
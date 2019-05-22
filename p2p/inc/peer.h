#ifndef _PEER_H_
#define _PEER_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <thread>

#include "protocol.h"

class TPeer{
private:
    static int m_bits_size;
    static int m_peer_sock;
    struct sockaddr_in m_peer_addr;

    static void KeepAlive(int);
    static void Read();
    static void GetPeerList();
public:
    TPeer(int);
    TPeer();
    ~TPeer();

    void LogIn(std::string, int);   // init connection
                                    // register on the tracker
    void Run();                     // keep alive 
};

int TPeer::m_peer_sock;
int TPeer::m_bits_size;

TPeer::TPeer(int _bits_size){
    this->m_peer_sock = socket(AF_INET, SOCK_STREAM, 0);
    m_bits_size = _bits_size;
}

TPeer::TPeer(){

}

void TPeer::LogIn(std::string _ip, int _port){
    if(m_peer_sock < 0){
        perror("Can not Create Socket!");
        exit(0);
    }

    memset(&m_peer_addr, 0, sizeof(m_peer_addr));
    m_peer_addr.sin_family = AF_INET;
    m_peer_addr.sin_port = htons(_port);
 
    int res = inet_pton(AF_INET, _ip.c_str(), &m_peer_addr.sin_addr);
 
    if (0 > res){
        perror("error: first parameter is not a valid address family");
        close(m_peer_sock);
        exit(EXIT_FAILURE);
    }
    else if (0 == res){
        perror("char string (second parameter does not contain valid ipaddress");
        close(m_peer_sock);
        exit(EXIT_FAILURE);
    }

    if (connect(m_peer_sock, (const struct sockaddr *)&m_peer_addr, sizeof(m_peer_addr)) < 0){
        perror("connect failed"); 
        exit(1);
    }

    std::cout << "Client Connected\n";
}

void TPeer::KeepAlive(int _sleep){
    std::string text;
    TProtocol mtcp(m_bits_size);

    std::vector<std::string> tmp = {"Lorem ipsum dolor sit amet, consectetur adipisicing elit. Temporibus dignissimos inc.", "Hello maldonado, how are you?!!", "Esto es una prueba del protocolo desarrollado"};

    srand(time(NULL));
    while(true){        
        text = "on";
        mtcp.Sending(text, m_peer_sock);
        std::this_thread::sleep_for(std::chrono::milliseconds(_sleep));
    }
}

void TPeer::Read(){
    unsigned buffer_size = 1;
    char buffer[buffer_size];

    int n;

    while(true){
        memset(&buffer, 0, buffer_size);
        n = recv(m_peer_sock, buffer, buffer_size, 0);
        if(n == 0){         
            break;
        }
        else if(n < 0){
            perror("error receiving text");
        }
        else{
            std::cout << buffer << "\n";
        }       
    }
}

void TPeer::GetPeerList(){
    
}

void TPeer::Run(){
    int tsleep = 2500;
    std::thread talive(KeepAlive, tsleep);  // Keep Alive
    std::thread tread(Read);                // Read Message
    std::thread tgetlist(GetPeerList);      // Get Peer List

    talive.join();
    tread.join();
    tgetlist.join();

    close(m_peer_sock);
}


TPeer::~TPeer(){

}
#endif
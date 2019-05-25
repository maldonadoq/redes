#ifndef _PEER_H_
#define _PEER_H_

#include <boost/algorithm/string.hpp> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <future>

#include "protocol.h"
#include "peer-info.h"
#include "utils.h"

class TPeer{
private:
    static std::vector<TPeerInfo> m_peers;

    static int m_bits_size;
    static int m_peer_sock;
    struct sockaddr_in m_peer_addr;

    static void KeepAlive(int);
    static void PeerLeft(TPeerInfo);
    static void PeerJoin(TPeerInfo);
    static void Testing();
    static void Listening();
    static void PrintPeers();

    void GetPeerList();
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
std::vector<TPeerInfo> TPeer::m_peers;

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
    GetPeerList();
}

void TPeer::KeepAlive(int _sleep){
    std::string text;
    TProtocol mtcp(m_bits_size);
    while(m_peer_sock > 0){
        text = "on";
        mtcp.Sending(text, m_peer_sock, "K");
        std::this_thread::sleep_for(std::chrono::milliseconds(_sleep));
    }
}

void TPeer::PrintPeers(){
    std::cout << "list of peers ip: ";
    for(unsigned i=0; i<m_peers.size(); i++){
        std::cout << m_peers[i].m_ip << " ";
    }
    std::cout << "\n";
}

void TPeer::Listening(){
    TProtocol mtcp(m_bits_size);
    std::string text;
    bool tstate = true;

    TPeerInfo pinfo;
    while(tstate){
        text = mtcp.Receiving(m_peer_sock);
        switch(text[0]){
            case 'O':
            case 'o':{
                tstate = false;
                break;
            }
            case 'R':
            case 'r':{
                std::cout << "\nremove: " << text.substr(1) << "\n";
                pinfo = {text.substr(1), 0};
                PeerLeft(pinfo);
                PrintPeers();
                break;
            }
            case 'J':
            case 'j':{
                std::cout << "\njoin: " << text.substr(1) << "\n";
                // excluir la misma ip
                pinfo = {text.substr(1), 0};
                PeerJoin(pinfo);
                PrintPeers();
                break;
            }
        }
    }
}

void TPeer::Testing(){
    TProtocol mtcp(m_bits_size);
    char _type;
    bool state = true;
    while(state){
        _type = getch();

        switch(_type){
            case 'O':
            case 'o':{                
                state = false;
                mtcp.Sending("LogOut", m_peer_sock, "O");
                std::cout << "LogOut\n";
                m_peer_sock = -1;
                break;
            }
        }
    }
}

void TPeer::GetPeerList(){
    std::string rtext;
    std::string rlist;
    TProtocol mtcp(m_bits_size);
    rtext = "List";

    std::thread tsend_request(mtcp.Sending, rtext, m_peer_sock, "L");
    auto future = std::async(mtcp.Receiving, m_peer_sock);

    tsend_request.join();
    rlist = future.get();

    if(rlist.size() > 0){
        std::vector<std::string> result;
        boost::split(result, rlist, boost::is_any_of("|")); 

        TPeerInfo pinfo;
        std::cout << "list of peers: ";
        for(unsigned i=0; i<result.size(); i++){
            pinfo = {result[i], 0};
            m_peers.push_back(pinfo);

            std::cout << result[i] << " ";
        }
        std::cout << "\n";
    }    
}

void TPeer::PeerJoin(TPeerInfo _peer){
    m_peers.push_back(_peer);
}

void TPeer::PeerLeft(TPeerInfo _peer){
    std::vector<TPeerInfo> peers_tmp;
    for(unsigned i=0; i<m_peers.size(); i++){
        if(m_peers[i].m_ip != _peer.m_ip){
            peers_tmp.push_back(m_peers[i]);
        }
    }

    m_peers = peers_tmp;
}

void TPeer::Run(){
    int tsleep = 2500;
    std::thread talive(KeepAlive, tsleep);  // Keep Alive
    std::thread ttest(Testing);             // Testing
    std::thread tlistenng(Listening);       // Testing

    talive.join();
    ttest.join();
    tlistenng.join();

    close(m_peer_sock);
    m_peers.clear();
}


TPeer::~TPeer(){

}
#endif
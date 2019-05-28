#ifndef _PEER_H_
#define _PEER_H_

#include <boost/algorithm/string.hpp> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <mutex>
#include <map>
#include <unistd.h>
#include <math.h>

#include "protocol.h"
#include "peer-info.h"
#include "utils.h"

using std::cout;
using std::vector;
using std::map;
using std::string;
using std::to_string;
using std::thread;

/*
    Client Testing Commands [Press Key!!]:
        L/l -> Login
        O/o -> Logout
        G/g -> Get Peer List
*/

std::mutex  gmutex;

const string files_name[6] = {"file/file1.txt", "file/file2.txt", "file/file3.txt", "file/file4.txt"};
class TPeer{
private:
    static map<string, vector<string> > m_chunk_files;
    static vector<string> m_file_complete;
    static int m_chunk_size;
    static bool m_state;

    static vector<TPeerInfo> m_neighboring_peers;   // list of neightbor peer
    static TPeerInfo m_tracker_info;                // tracker ip - port
    static TPeerInfo m_peer_info;                   // peer ip - port

    static int m_bits_size;                         // block of messages
    static int m_peer_server_sock;                  // server sock
    struct sockaddr_in m_peer_server_addr;          // server address
    
    // Server
    static void SPeerLeft(TPeerInfo);
    static void SPeerJoin(TPeerInfo);
    static void SPeerListJoin(vector<string>);    
    static void SListening();
    static void SUpload(vector<string>);
    static void SDownload(vector<string>);

    // Client
    static void CConnectAndSend(TPeerInfo, string, string);
    static void CTesting();
    static void CUpload(string, string);
    static void CDownload(string);

    void Init();
    static void PrintPeers();
    static void AddBlock(string, vector<string>);
public:
    TPeer(string, int, int);
    TPeer();
    ~TPeer();

    void Execute();
};

TPeerInfo   TPeer::m_peer_info;
TPeerInfo   TPeer::m_tracker_info;
int         TPeer::m_peer_server_sock;
int         TPeer::m_bits_size;
int         TPeer::m_chunk_size;
bool        TPeer::m_state;

map<string, vector<string> >    TPeer::m_chunk_files;
vector<TPeerInfo>               TPeer::m_neighboring_peers;
vector<string>                  TPeer::m_file_complete;

TPeer::TPeer(string _tracker_ip, int _tracker_port, int _bits_size){
    m_state         = true;
    m_bits_size     = _bits_size;
    m_chunk_size    = 32;

    // Client    
    m_tracker_info  = {_tracker_ip, _tracker_port};    

    // Server
    m_peer_info     = {getIPAddress(), getpid()};
    this->m_peer_server_sock = socket(AF_INET, SOCK_STREAM, 0);
    Init();
}

TPeer::TPeer(){

}

void TPeer::Init(){
    int reuse = 1;
    memset(&m_peer_server_addr, 0, sizeof(sockaddr_in));

    m_peer_server_addr.sin_family = AF_INET;
    m_peer_server_addr.sin_addr.s_addr = INADDR_ANY;
    m_peer_server_addr.sin_port = htons(m_peer_info.m_port);

    //Avoid bind error if the socket was not close()
    setsockopt(m_peer_server_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));

    int nr = bind(m_peer_server_sock, (struct sockaddr *) &m_peer_server_addr, sizeof(sockaddr_in));
    if(nr < 0){
        perror("Failed to bind");
    }

    listen(m_peer_server_sock, 10);
    cout << "Peer-Server Created!\n";
}

void TPeer::CDownload(string _file){
    cout << "Client Download " << _file << "\n";

    for(unsigned i=0; i<m_neighboring_peers.size(); i++){
        CConnectAndSend(m_neighboring_peers[i],_file+"|"+PeerToStr(m_peer_info), "D");
    }
}

void TPeer::SDownload(vector<string> _parse){
    if(_parse.size() == 3){
        string _key = _parse[0];

        cout << "\n\nServer Download " << _key << "\n";

        _parse.erase(_parse.begin());
        TPeerInfo _pinfo = MakePeerInfo(_parse);

        std::map<string, vector<string> >::iterator it;
        it = m_chunk_files.find(_key);

        string message = "Does not exist!";
        if(it != m_chunk_files.end()){
            message = "";
            for(unsigned i=0; i<(it->second).size(); i++){
                message += "-" + (it->second)[i];
            }
            message = message.substr(1);
        }

        CConnectAndSend(_pinfo, message, "R");
    }
    else{
        cout << "[key, ip, port]\n";
    }
}

void TPeer::CConnectAndSend(TPeerInfo _machine,
    string _message, string _type){

    int peer_clt_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(peer_clt_sock < 0){
        perror("Can not Create Socket!");
        exit(0);
    }

    struct sockaddr_in peer_clt_addr;

    memset(&peer_clt_addr, 0, sizeof(peer_clt_addr));
    peer_clt_addr.sin_family = AF_INET;
    peer_clt_addr.sin_port = htons(_machine.m_port);
 
    int nr = inet_pton(AF_INET, _machine.m_ip.c_str(), &peer_clt_addr.sin_addr);
 
    if(nr <= 0){
        perror("error: Not a valid address family");
        close(peer_clt_sock);
        exit(0);
    }
    /*
                                   >
                                   |
    Connection Established ---------

    Sending Message ----------------
                                   |
                                   <
    */

    nr = -1;
    do{
        nr = connect(peer_clt_sock, (const struct sockaddr *)&peer_clt_addr, sizeof(struct sockaddr_in));
        if(nr >= 0){
            TProtocol mtcp(m_bits_size);
            mtcp.Sending(_message, peer_clt_sock, _type);
        }
    }while(nr < 0);

    shutdown(peer_clt_sock, SHUT_RDWR);
    close(peer_clt_sock);
}

void TPeer::CTesting(){
    char cmmd;
    string message, tfile;
    int nfile;
    while(m_state){
        cmmd = getch();
        switch(cmmd){
            // Peer Client
            case 'G':
            case 'g':{
                message = PeerToStr(m_peer_info);
                CConnectAndSend(m_tracker_info,message,"G");
                break;
            }
            case 'L':
            case 'l':{
                message = PeerToStr(m_peer_info);
                CConnectAndSend(m_tracker_info,message,"L");
                break;
            }
            case 'O':
            case 'o':{
                message = PeerToStr(m_peer_info);
                CConnectAndSend(m_tracker_info,message,"O");
                /*gmutex.lock();
                    m_state = false;
                gmutex.unlock();*/
                break;
            }
            default:
                if(cmmd >= '1' and cmmd <= '4'){
                    nfile = cmmd - '0' - 1;
                    tfile = ReadFile(files_name[nfile]);
                    CUpload(files_name[nfile], tfile);
                }
                else if(cmmd >= '5' and cmmd <= '8'){
                    nfile = cmmd - '0' - 5;
                    CDownload(files_name[nfile]);
                }
                break;
        }
    }
}

void TPeer::AddBlock(string _key, vector<string> _chunk){
    std::map<string, vector<string> >::iterator it;
    it = m_chunk_files.find(_key);

    if(it != m_chunk_files.end()){
        cout << "finded\n";
        for(unsigned i=0; i<_chunk.size(); i++){
            (it->second).push_back(_chunk[i]);
        }

        cout << "added: " << _chunk.size() << "\n";
    }
    else{
        cout << "init\n";
        m_chunk_files[_key] = _chunk;
        cout << "added: " << _chunk.size() << "\n";
    }
}

void TPeer::SUpload(vector<string> _parse){
    if(_parse.size() > 1){
        string _key = _parse[0];
        _parse.erase(_parse.begin());
        cout << "key: " << _key << "\n";
        AddBlock(_key, _parse);
    }
}

void TPeer::CUpload(string _file_key ,string _file_body){
    vector<string> nchunk = SplitText(_file_body, m_chunk_size);

    // print_vector(nchunk);
    cout << nchunk.size() << "\n";

    if(m_neighboring_peers.size() != 0){
        int nbpp = (int)nchunk.size()/((int)m_neighboring_peers.size()+1);
        int mbpp = (int)nchunk.size()%((int)m_neighboring_peers.size()+1);

        nbpp += (mbpp == 0)? 0: 1;
        cout << "each peer receive: " << nbpp << "\n";

        vector<string> tmp;
        for(unsigned i=0; i<nbpp; i++){
            tmp.push_back(nchunk[i]);
        }
        AddBlock(_file_key, tmp);

        int k = 0;
        string ablock;
        for(unsigned i=nbpp; i<nchunk.size(); i+=nbpp){
            // cout << "send to neightbor " << k << ": ";
            ablock = _file_key;
            for(unsigned j=0; j<nbpp and (i+j)<nchunk.size(); j++){
                // cout << j+i << " ";
                ablock += "|" + nchunk[i+j];
            }            
            // cout << ablock.substr(1) << "\n";
            CConnectAndSend(m_neighboring_peers[k], ablock, "U");
            k++;
        }
    }
    else{
        AddBlock(_file_key, nchunk);
    }
}

void TPeer::Execute(){
    thread ttest(CTesting);
    thread tlisten(SListening);

    ttest.join();
    tlisten.join();

    close(m_peer_server_sock);
}

void TPeer::SListening(){
    socklen_t cli_size = sizeof(sockaddr_in);
    struct sockaddr_in peer_addr;
    int ConnectSock;
    string command, message;
    vector<string> vparse;

    TProtocol mtcp(m_bits_size);
    TPeerInfo pinfo;

    while(m_state){
        ConnectSock = accept(m_peer_server_sock, (struct sockaddr *) &peer_addr, &cli_size);
        if(ConnectSock < 0)
            perror("Error on accept");
        else{           
            // printf("address: %s\n", inet_ntoa(peer_addr.sin_addr));
            // printf("port %d\n", ntohs(peer_addr.sin_port));
            command = mtcp.Receiving(ConnectSock);
            cout << command << "\n";
            switch(command[0]){
                // Peer Server
                case 'G':
                case 'g':{
                    cout << "Set Peer List\n";
                    vparse = SplitMessage(command.substr(1), "|");
                    SPeerListJoin(vparse);
                    break;
                }
                case 'J':
                case 'j':{
                    cout << "Peer Join\n";
                    vparse = SplitMessage(command.substr(1), "|");
                    pinfo = MakePeerInfo(vparse);
                    SPeerJoin(pinfo);
                    break;
                }
                case 'L':
                case 'l':{
                    cout << "Peer Remove\n";
                    vparse = SplitMessage(command.substr(1), "|");
                    pinfo = MakePeerInfo(vparse);
                    SPeerLeft(pinfo);
                    break;
                }
                case 'K':
                case 'k':{
                    cout << "I'm Okay!\n";
                    message = PeerToStr(m_peer_info);
                    CConnectAndSend(m_tracker_info,message,"K");
                    break;
                }
                case 'U':
                case 'u':{
                    cout << "Upload\n";
                    vparse = SplitMessage(command.substr(1), "|");
                    SUpload(vparse);
                    break;
                }
                case 'D':
                case 'd':{
                    cout << "Download\n";
                    vparse = SplitMessage(command.substr(1), "|");
                    SDownload(vparse);
                    break;
                }
                case 'R':
                case 'r':{
                    cout << "Receive Download\n";
                    break;
                }
                default:
                    break;
            }
        }
        shutdown(ConnectSock, SHUT_RDWR);
        close(ConnectSock);
    }

    shutdown(m_peer_server_sock, SHUT_RDWR);
    close(m_peer_server_sock);
}

void TPeer::SPeerListJoin(vector<string> _parse){
    if((_parse.size() > 2) and ((int)_parse.size()%2 == 0)){
        TPeerInfo pinfo;
        for(unsigned i=0; i<_parse.size(); i+=2){
            pinfo = {_parse[i], stoi(_parse[i+1])};
            if(pinfo.m_port != m_peer_info.m_port){
                SPeerJoin(pinfo);
            }
        }
        PrintPeers();
    }
    else{
        cout << "List of Peer Must be Even! [ip,port]\n";
    }
}

void TPeer::SPeerJoin(TPeerInfo _peer){
    if(_peer.m_port > 0){
        m_neighboring_peers.push_back(_peer);
    }
    else{
        cout << "Peer Port Must be Greater than 0!\n";
    }
}

void TPeer::SPeerLeft(TPeerInfo _peer){
    if(_peer.m_port > 0){
        vector<TPeerInfo> peers_tmp;
        for(unsigned i=0; i<m_neighboring_peers.size(); i++){
            if(m_neighboring_peers[i].m_port != _peer.m_port){
                peers_tmp.push_back(m_neighboring_peers[i]);
            }
        }

        m_neighboring_peers = peers_tmp;
    }
    else{
        cout << "Peer Port Must be Greater than 0!\n";
    } 
}

void TPeer::PrintPeers(){
    cout << "Peers's List [ip - port]\n";
    for(unsigned i=0; i<m_neighboring_peers.size(); i++){
        cout << m_neighboring_peers[i].m_ip << " - " << m_neighboring_peers[i].m_port << "\n";
    }
}

TPeer::~TPeer(){

}
#endif
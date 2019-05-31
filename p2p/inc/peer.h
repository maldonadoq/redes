#ifndef _PEER_H_
#define _PEER_H_

#include <boost/algorithm/string.hpp> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <algorithm>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <mutex>
#include <map>
#include <unistd.h>
#include <math.h>
#include <utility>
#include <fstream>

#include "protocol.h"
#include "peer-info.h"
#include "utils.h"

using std::cout;
using std::cin;
using std::vector;
using std::map;
using std::sort;
using std::string;
using std::to_string;
using std::thread;
using std::mutex;
using std::pair;
using std::make_pair;
using std::ofstream;
/*
    Client Testing Commands [Press Key!!]:
        L/l -> Login
        O/o -> Logout
        G/g -> Get Peer List
*/

std::mutex  gmutex;

const string files_name[6] = {"file1.txt", "file2.txt", "file3.txt", "file4.txt"};
class TPeer{
private:
    static map<string, vector<string> > m_chunk_files;
    static vector<string> m_file_complete;
    static int m_chunk_size;
    static int m_file_number;
    static bool m_state;
    static mutex m_cmutex;

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
    static void SDownloadComplete(string);
    static void SSave(string);

    // Client
    static void CConnectAndSend(TPeerInfo, string, string);
    static void CTesting();
    static void CUpload(string, string);
    static void CDownload(string, int);

    void Init();
    static void PrintPeers();
    static void AddBlock(string, vector<string>);
    static void AddFile(string);
public:
    TPeer(string, int, int);
    TPeer();
    ~TPeer();

    void Execute();
};

mutex       TPeer::m_cmutex;
TPeerInfo   TPeer::m_peer_info;
TPeerInfo   TPeer::m_tracker_info;
int         TPeer::m_peer_server_sock;
int         TPeer::m_bits_size;
int         TPeer::m_chunk_size;
int         TPeer::m_file_number;
bool        TPeer::m_state;

map<string, vector<string> >    TPeer::m_chunk_files;
vector<TPeerInfo>               TPeer::m_neighboring_peers;
vector<string>                  TPeer::m_file_complete;

TPeer::TPeer(string _tracker_ip, int _tracker_port, int _bits_size){
    m_state         = true;
    m_bits_size     = _bits_size;
    m_chunk_size    = 256;

    m_chunk_files.clear();

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

    listen(m_peer_server_sock, 20);
    cout << "Peer-Server Created!\n";
}

void TPeer::CDownload(string _file, int _number){
    int ts = m_neighboring_peers.size();
    // for(int i=0; i<ts and i<_number; i++){
        // CConnectAndSend(m_neighboring_peers[rand()%ts],_file+"|"+PeerToStr(m_peer_info), "D");
    for(int i=0; i<ts; i++){
        CConnectAndSend(m_neighboring_peers[i],_file+"|"+PeerToStr(m_peer_info), "D");
    }
}

void TPeer::SDownload(vector<string> _parse){
    if(_parse.size() == 3){
        string _key = _parse[0];
        cout << "\nServer: Download\n";
        cout << "  key: " << _key << "\n";

        _parse.erase(_parse.begin());
        TPeerInfo _pinfo = MakePeerInfo(_parse);

        std::map<string, vector<string> >::iterator it;
        it = m_chunk_files.find(_key);

        string message = "Not";
        if(it != m_chunk_files.end()){
            message = "";
            for(unsigned i=0; i<(it->second).size(); i++){
                message += "|" + (it->second)[i];
            }
            message = message.substr(1);
            cout << "  find: true\n";
        }
        else{
            cout << "  find: false\n";
        }

        CConnectAndSend(_pinfo, message, "R");
    }
    else{
        cout << "  [key, ip, port]\n";
    }
}

void TPeer::SDownloadComplete(string _key){
    bool state = true, flag = true;

    std::map<string, vector<string> >::iterator it;
    it = m_chunk_files.find(_key);

    if(it != m_chunk_files.end()){
        for(unsigned i=0; i<(it->second).size(); i++){
            m_file_complete.push_back((it->second)[i]);
        }

        if(m_file_complete.size() > 0){
            vector<string> block = SplitMessage(m_file_complete[0], "/");
            m_file_number = stoi(block[1]);
        }
    }

    while(state){
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        if(m_file_complete.size() == m_file_number){
            state = false;
        }
        else if(m_file_complete.size() > 0 and flag){
            vector<string> block = SplitMessage(m_file_complete[0], "/");
            // cout << block[1] << "\n";
            m_file_number = stoi(block[1]);
            flag = false;
        }
        cout << "  Download: " << m_file_complete.size() << "/" << m_file_number << "\n";
    }

    SSave(_key);
    // print_vector(m_file_complete);
}

void TPeer::SSave(string _key){
    vector<string> block;
    vector<pair<int, int> > block_pair;
    for(unsigned i=0; i<m_file_complete.size(); i++){
        block = SplitMessage(m_file_complete[i], "/");
        m_file_complete[i] = block[2];

        block_pair.push_back(make_pair(stoi(block[0]), i));
    }

    sort(block_pair.begin(), block_pair.end(), compare_pair);
    
    ofstream file("download/" + _key);

    for(unsigned i=0; i<block_pair.size(); i++){
        file << m_file_complete[block_pair[i].second];
    }

    file.close();
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
            /*
            case 'G':
            case 'g':{
                cout << "\nClient: Get Peer List\n";
                message = PeerToStr(m_peer_info);
                CConnectAndSend(m_tracker_info,message,"G");
                break;
            }*/
            case 'L':
            case 'l':{
                cout << "\nClient: Login\n";
                message = PeerToStr(m_peer_info);
                CConnectAndSend(m_tracker_info,message,"L");
                break;
            }
            case 'O':
            case 'o':{
                cout << "\nClient: Logout\n";
                message = PeerToStr(m_peer_info);
                CConnectAndSend(m_tracker_info,message,"O");
                /*gmutex.lock();
                    m_state = false;
                gmutex.unlock();*/
                break;
            }
            case 'U':
            case 'u':{
                cout << "\nClient: Upload\n";
                cout << "  File to Load: ";
                cin >> message;

                tfile = ReadFile("upload/"+message);
                CUpload(message, tfile);
                break;
            }
            case 'D':
            case 'd':{
                cout << "\nClient: Download\n";
                cout << "  File to Download: ";
                cin >> message;
                CDownload(message, 3);
                m_file_complete.clear();
                thread tdownload_complete(SDownloadComplete, message);
                tdownload_complete.detach();
                break;
            }
            default:
                if(cmmd >= '1' and cmmd <= '4'){
                    nfile = cmmd - '0' - 1;
                    cout << "\nClient: Upload " << files_name[nfile] << "\n";
                    tfile = ReadFile("upload/"+files_name[nfile]);
                    CUpload(files_name[nfile], tfile);
                }
                else if(cmmd >= '5' and cmmd <= '8'){
                    nfile = cmmd - '0' - 5;
                    cout << "\nClient: Download " << files_name[nfile] << "\n";
                    CDownload(files_name[nfile], 3);
                    m_file_complete.clear();
                    thread tdownload_complete(SDownloadComplete, files_name[nfile]);
                    tdownload_complete.detach();
                }
                break;
        }
    }
}

void TPeer::AddBlock(string _key, vector<string> _chunk){
    std::map<string, vector<string> >::iterator it;
    it = m_chunk_files.find(_key);

    if(it != m_chunk_files.end()){
        cout << "    File Exist\n";
        for(unsigned i=0; i<_chunk.size(); i++){
            (it->second).push_back(_chunk[i]);
        }
    }
    else{
        cout << "    File Created\n";
        m_chunk_files[_key] = _chunk;
    }
}

void TPeer::SUpload(vector<string> _parse){
    if(_parse.size() > 1){
        string _key = _parse[0];
        cout << "\nServer: Upload\n";
        _parse.erase(_parse.begin());
        cout << "  key: " << _key << "\n";
        AddBlock(_key, _parse);
    }
}

void TPeer::CUpload(string _file_key ,string _file_body){
    vector<string> nchunk = SplitText(_file_body, m_chunk_size);

    // print_vector(nchunk);
    // cout << nchunk.size() << "\n";

    if(m_neighboring_peers.size() != 0){
        int nbpp = (int)nchunk.size()/((int)m_neighboring_peers.size()+1);
        int mbpp = (int)nchunk.size()%((int)m_neighboring_peers.size()+1);

        nbpp += (mbpp == 0)? 0: 1;
        // cout << "  Each peer receive: " << nbpp << "\n";

        vector<string> tmp;
        for(unsigned i=0; i<nbpp; i++){
            tmp.push_back(nchunk[i]);
        }
        AddBlock(_file_key, tmp);

        int k = 0;
        string ablock;
        for(unsigned i=nbpp; i<nchunk.size(); i+=nbpp){
            cout << "  Sending to: ";
            PrintPeer(m_neighboring_peers[k]);
            cout << "\n";

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
            switch(command[0]){
                // Peer Server
                case 'G':
                case 'g':{
                    cout << "\nServer: Get Peer List\n";
                    vparse = SplitMessage(command.substr(1), "|");
                    SPeerListJoin(vparse);
                    break;
                }
                case 'J':
                case 'j':{
                    cout << "\nServer: Peer Join\n";
                    vparse = SplitMessage(command.substr(1), "|");
                    pinfo = MakePeerInfo(vparse);
                    SPeerJoin(pinfo);
                    break;
                }
                case 'L':
                case 'l':{
                    cout << "\nServer: Peer Left\n";
                    vparse = SplitMessage(command.substr(1), "|");
                    pinfo = MakePeerInfo(vparse);
                    SPeerLeft(pinfo);
                    break;
                }
                case 'K':
                case 'k':{
                    // cout << "Server: Keep Alive\n";
                    message = PeerToStr(m_peer_info);
                    CConnectAndSend(m_tracker_info,message,"K");
                    break;
                }
                case 'U':
                case 'u':{
                    vparse = SplitMessage(command.substr(1), "|");
                    SUpload(vparse);
                    break;
                }
                case 'D':
                case 'd':{
                    vparse = SplitMessage(command.substr(1), "|");
                    SDownload(vparse);
                    break;
                }
                case 'R':
                case 'r':{
                    cout << "Server: Receive Download\n";
                    AddFile(command.substr(1));
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

void TPeer::AddFile(string _bfile){
    if(_bfile != "Not"){
        // cout << "request: " << _bfile << "\n";
        vector<string> vb = SplitMessage(_bfile,"|");

        m_cmutex.lock();
            for(unsigned i=0; i<vb.size(); i++){
                m_file_complete.push_back(vb[i]);
            }
        m_cmutex.unlock();
    }
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
        cout << "  N[ip,port]\n";
    }
}

void TPeer::SPeerJoin(TPeerInfo _peer){
    if(_peer.m_port > 0){
        m_neighboring_peers.push_back(_peer);
    }
    else{
        cout << "  Peer port > 0!\n";
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
        cout << "  Peer Port > 0!\n";
    } 
}

void TPeer::PrintPeers(){
    cout << "  Peers's List [ip - port]\n";
    for(unsigned i=0; i<m_neighboring_peers.size(); i++){
        cout << "    ";
        PrintPeer(m_neighboring_peers[i]);
        cout << "\n";
    }
}

TPeer::~TPeer(){

}

#endif
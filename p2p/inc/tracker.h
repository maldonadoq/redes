#ifndef _TRACKER_H_
#define _TRACKER_H_

#include <iostream>
#include <vector>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <chrono>

#include "peer-info.h"
#include "protocol.h"

using std::cout;
using std::vector;
using std::string;
using std::thread;
using std::mutex;

class TTracker{
private:
	static int m_bits_size;
	static vector<TPeerInfo> m_peers;

	static mutex m_cmutex;

	struct sockaddr_in m_tracker_addr;
	static int m_tracker_sock;

	// Server
	static bool SPeerJoin(TPeerInfo);
	static bool SPeerLeft(TPeerInfo);
	static void SListening();
	static void SKeepAlive(TPeerInfo);

	// Client
	static void CConnectAndSend(TPeerInfo, string, string);
	static void CSendMessage(TPeerInfo, string, string);
	static void CSendToAll(TPeerInfo, string, string);
	static void CSendToAll(string, string);
	static void CTesting(int);
	static void CKeepAlive();

	static int  PeerFind(TPeerInfo);
	static void KeepAliveRestart();
	static string GetPeerList();	
public:
	TTracker();
	TTracker(int, int);
	~TTracker();

	void Create(int);
	void Execute();
};

vector<TPeerInfo> 	TTracker::m_peers;
int 				TTracker::m_tracker_sock;
int 				TTracker::m_bits_size;
mutex 				TTracker::m_cmutex;

TTracker::TTracker(){
	m_tracker_sock = socket(AF_INET, SOCK_STREAM, 0);
}

TTracker::TTracker(int _bits_size, int _port){
	m_tracker_sock = socket(AF_INET, SOCK_STREAM, 0);
	m_bits_size = _bits_size;

	Create(_port);
}

void TTracker::Create(int _port){
	int reuse = 1;
    memset(&m_tracker_addr, 0, sizeof(sockaddr_in));

    m_tracker_addr.sin_family = AF_INET;
    m_tracker_addr.sin_addr.s_addr = INADDR_ANY;
    m_tracker_addr.sin_port = htons(_port);

    //Avoid bind error if the socket was not close()
    setsockopt(m_tracker_sock,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));

    if(bind(m_tracker_sock, (struct sockaddr *) &m_tracker_addr, sizeof(sockaddr_in)) < 0)
        perror("Failed to bind");

    listen(m_tracker_sock, 20);
    cout << "Server Created!\n";
}

void TTracker::SListening(){
	socklen_t cli_size = sizeof(sockaddr_in);
	struct sockaddr_in peer_addr;
	int ConnectSock;
	bool state = true;
	string command, message;
	vector<string> vparse;

	TProtocol mtcp(m_bits_size);
	TPeerInfo tinfo;

	while(state){
		ConnectSock = accept(m_tracker_sock, (struct sockaddr *) &peer_addr, &cli_size);

	    if(ConnectSock < 0)
	        perror("Error on accept");
	    else{	    	
	    	// printf("address: %s\n", inet_ntoa(peer_addr.sin_addr));
	    	// printf("port %d\n", ntohs(peer_addr.sin_port));
	    	command = mtcp.Receiving(ConnectSock);
	    	// cout << command << "\n";
	    	switch(command[0]){
	    		// Peer Client
	    		case 'G':
	    		case 'g':{
	    			cout << "\nServer: Get Peer List\n";
	    			vparse = SplitMessage(command.substr(1), "|");
	    			tinfo = MakePeerInfo(vparse);
	    			message = GetPeerList();
	    			CSendMessage(tinfo, message, "G");
	    			break;
	    		}
	    		case 'L':
	    		case 'l':{
	    			cout << "\nServer: Peer Join\n";
	    			vparse = SplitMessage(command.substr(1), "|");
	    			tinfo = MakePeerInfo(vparse);
	    			if(SPeerJoin(tinfo)){
	    				// Send to All that one Peer Join
	    				message = GetPeerList();
	    				CSendMessage(tinfo, message, "G");
	    				
		    			CSendToAll(tinfo, command.substr(1),"J");
	    			}		    			
	    			break;
	    		}
	    		case 'O':
	    		case 'o':{
	    			cout << "\nServer: Peer Left\n";
	    			vparse = SplitMessage(command.substr(1), "|");
	    			tinfo = MakePeerInfo(vparse);
	    			if(SPeerLeft(tinfo)){
	    				// Send to All that one Peer Join
		    			CSendToAll(tinfo, command.substr(1),"L");
	    			}

	    			// Send to All that one Peer Left
	    			break;
	    		}
	    		case 'K':
	    		case 'k':{
	    			cout << "\nServer: Keep Alive\n";
	    			vparse = SplitMessage(command.substr(1), "|");
	    			tinfo = MakePeerInfo(vparse);
	    			SKeepAlive(tinfo);
	    			// cout << PeerToStr(tinfo) << "\n";
	    			break;
	    		}
	    		default:
	    			break;
	    	}
	    }

	    shutdown(ConnectSock, SHUT_RDWR);
		close(ConnectSock);
	}

	shutdown(m_tracker_sock, SHUT_RDWR);
	close(m_tracker_sock);
}

int TTracker::PeerFind(TPeerInfo _pinfo){
	for(unsigned i=0; i<m_peers.size(); i++){
		if(m_peers[i].m_port == _pinfo.m_port){
			return i;
		}
	}

	return -1;
}

bool TTracker::SPeerJoin(TPeerInfo _pinfo){
	if((_pinfo.m_port > 0) and (PeerFind(_pinfo) < 0)){
		m_peers.push_back(_pinfo);
		cout << "  Peer Joined\n";
		return true;
	}
	
	cout << "  Peer Already Exist\n";
	return false;
}

bool TTracker::SPeerLeft(TPeerInfo _pinfo){
	int idx = PeerFind(_pinfo);
	if(idx >= 0){
		m_peers.erase(m_peers.begin() + idx);
		cout << "  Peer Remove\n";
		return true;
	}
	cout << "  Peer Does Not Exist\n";
	return false;
}

void TTracker::CSendMessage(TPeerInfo _pinfo, string _message, string _type){
	if((_pinfo.m_port > 0) and (PeerFind(_pinfo) >= 0)){
		CConnectAndSend(_pinfo, _message, _type);
	}
	else{
		cout << "  Peer Does Not Exist\n";
	}
}

string TTracker::GetPeerList(){
	string speers = "";
	unsigned tsize = m_peers.size();

	if(tsize == 1){
		speers = PeerToStr(m_peers[0]);
	}
	else if(tsize > 1){
		for(unsigned i=0; i<tsize-1; i++){
			speers += PeerToStr(m_peers[i]) + "|";
		}

		speers += PeerToStr(m_peers[tsize-1]);
	}

	return speers;
}



void TTracker::CTesting(int _time){

	bool state = true;
	TProtocol mtcp(m_bits_size);
	while(state){
		// cout << "Send Keep Alive\n";
		CSendToAll("Are you ok?", "K");
		KeepAliveRestart();
		std::this_thread::sleep_for(std::chrono::milliseconds(_time));
		CKeepAlive();
	}
}

void TTracker::SKeepAlive(TPeerInfo _pinfo){
	for(unsigned i=0; i<m_peers.size(); i++){
		if(m_peers[i].m_port == _pinfo.m_port){
			m_peers[i].m_keep = true;
			break;
		}
	}
}

void TTracker::KeepAliveRestart(){
	for(unsigned i=0; i<m_peers.size(); i++){
		m_peers[i].m_keep = false;
	}
}

void TTracker::CKeepAlive(){
	TTracker::m_cmutex.lock();

		vector<TPeerInfo> peers_tmp;
		vector<TPeerInfo> peers_rm;

	    for(unsigned i=0; i<m_peers.size(); i++){
	        if(m_peers[i].m_keep){
	            peers_tmp.push_back(m_peers[i]);
	            cout << PeerToStr(m_peers[i]) << " is Alive\n";
	        }
	        else{
	        	peers_rm.push_back(m_peers[i]);
	        }
	    }

	    m_peers = peers_tmp;

	TTracker::m_cmutex.unlock();

	for(unsigned i=0; i<peers_rm.size(); i++){
		CSendToAll(PeerToStr(peers_rm[i]), "L");
	}
}

void TTracker::CConnectAndSend(TPeerInfo _machine,
    string _message, string _type){

    int peer_clt_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(peer_clt_sock < 0){
        perror("Can not SCreate Socket!");
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

void TTracker::Execute(){
	// int ktime = 10000;
	thread tlisten(SListening);
	// thread ttest(CTesting, ktime);

	tlisten.join();
	// ttest.join();

	close(m_tracker_sock);
}

void TTracker::CSendToAll(TPeerInfo _pinfo, string _text, string _type){
	TTracker::m_cmutex.lock();
		
		for(unsigned i=0; i<m_peers.size(); i++){
			if(m_peers[i].m_port != _pinfo.m_port){
				CConnectAndSend(m_peers[i], _text, _type);
			}
		}

	TTracker::m_cmutex.unlock();
}

void TTracker::CSendToAll(string _text, string _type){
	TTracker::m_cmutex.lock();
		
		for(unsigned i=0; i<m_peers.size(); i++){			
			CConnectAndSend(m_peers[i], _text, _type);
		}

	TTracker::m_cmutex.unlock();
}

TTracker::~TTracker(){
	m_peers.clear();
}

#endif
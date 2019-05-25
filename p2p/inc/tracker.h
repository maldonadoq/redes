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

#include "socket-info.h"
#include "thread-u.h"
#include "protocol.h"

class TTracker{
private:
	static int m_bits_size;

	static std::mutex m_cmutex;
	static std::vector<TSocket> m_peers;

	struct sockaddr_in m_tracker_addr;
	static int m_tracker_sock;

	static void SendToAllPeers(std::string, std::string);
	static int  FindPeerIdx(TSocket *);	
	static void Disconnet(int);
	static std::string GetPeerList();
public:
	TTracker();
	TTracker(int);
	~TTracker();

	void Create(int);
	void Listening();
	static void HandlePeer(TSocket *);
};

std::vector<TSocket>	TTracker::m_peers;
int 					TTracker::m_tracker_sock;
int 					TTracker::m_bits_size;
std::mutex 				TTracker::m_cmutex;

TTracker::TTracker(){
	m_tracker_sock = socket(AF_INET, SOCK_STREAM, 0);
}

TTracker::TTracker(int _bits_size){
	m_tracker_sock = socket(AF_INET, SOCK_STREAM, 0);
	m_bits_size = _bits_size;
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

    listen(m_tracker_sock, 5);
    std::cout << "Server Created!\n";
}

void TTracker::Listening(){
	TSocket	*cli;
	TThread	*thr;

	socklen_t cli_size = sizeof(sockaddr_in);
	struct sockaddr_in m_clientAddr;

	int ip_unique = 0;

	while(true){
		cli = new TSocket();
		thr = new TThread();

		// block
		cli->m_sock = accept(m_tracker_sock, (struct sockaddr *) &m_clientAddr, &cli_size);

	    if(cli->m_sock < 0)
	        perror("Error on accept");
	    else{	    	
	    	// cli->SetName(inet_ntoa(m_clientAddr.sin_addr));

	    	// std::cout << m_clientAddr.
	    	printf("address: %s\n", inet_ntoa(m_clientAddr.sin_addr));
	    	printf("port %d\n", ntohs(m_clientAddr.sin_port));

	    	cli->SetIp(std::to_string(ip_unique));
	    	ip_unique++;
	        thr->Create(TTracker::HandlePeer, cli);
	    }
	}
}

std::string TTracker::GetPeerList(){
	std::string speers = "";
	unsigned tsize = m_peers.size();

	if(tsize == 1){
		speers = m_peers[0].m_ip;
	}
	else if(tsize >= 1){
		for(unsigned i=0; i<tsize-1; i++){
			speers += m_peers[i].m_ip + "|";
		}

		speers += m_peers[tsize-1].m_ip;
	}

	return speers;
}

void TTracker::HandlePeer(TSocket *cli){
	char buffer[m_bits_size];
	std::string text = "";

	int n;
	cli->m_state = true;

	std::cout << "ip client: " << cli->m_ip << " Created\n";

	SendToAllPeers(cli->m_ip, "J");

	TTracker::m_peers.push_back(*cli);
	std::string command, list_peer;

	// Init
	TProtocol mtcp(m_bits_size);
	command = mtcp.Receiving(cli->m_sock);

	if(command[0] == 'L' or command[0] == 'l'){
		list_peer = GetPeerList();
		mtcp.Sending(list_peer, cli->m_sock, "");
	}
	else{
		mtcp.Sending("", cli->m_sock, "");
	}	
	
	int idx;	
	while(cli->m_state){
		command = mtcp.Receiving(cli->m_sock);
		// std::cout << command << "\n";
		if(command.size() > 1){
			switch(command[0]){
				case 'K':
				case 'k':{
					// std::cout << "Keep Alive\n";
					break;
				}
				case 'O':
				case 'o':{
					std::cout << cli->m_ip << " Log Out\n";
					idx = FindPeerIdx(cli);
					cli->m_state = false;
					mtcp.Sending("LogOut", cli->m_sock, "O");
					Disconnet(idx);
					break;
				}
				default:
					break;
			}
		}
	}

	SendToAllPeers(cli->m_ip, "R");
	delete cli;
}

void TTracker::Disconnet(int idx){	
	TTracker::m_cmutex.lock();

		close(m_peers[idx].m_sock);
		TTracker::m_peers.erase(TTracker::m_peers.begin()+idx);

	TTracker::m_cmutex.unlock();
}

void TTracker::SendToAllPeers(std::string _text, std::string _type){
	TTracker::m_cmutex.lock();

		TProtocol mtcp(m_bits_size);
		for(unsigned i=0; i<m_peers.size(); i++){
			mtcp.Sending(_text, TTracker::m_peers[i].m_sock, _type);
		}

		// std::cout << "send to all\n";
	TTracker::m_cmutex.unlock();
}

int TTracker::FindPeerIdx(TSocket *_cli){
	for(unsigned i=0; i<m_peers.size(); i++)
		if(TTracker::m_peers[i].m_ip == _cli->m_ip)
			return i;

	return -1;
}

TTracker::~TTracker(){
	m_peers.clear();
}

#endif
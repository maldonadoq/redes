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

	static void SendToAllClients(std::string);
	static int  FindClientIdx(TSocket *);	
	static void Disconnet(int);
public:
	TTracker();
	TTracker(int);
	~TTracker();

	void Create(int);
	void Listening();
	static void HandleClient(TSocket *);
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

	while(true){
		cli = new TSocket();
		thr = new TThread();

		// block
		cli->m_sock = accept(m_tracker_sock, (struct sockaddr *) &m_clientAddr, &cli_size);

	    if(cli->m_sock < 0)
	        perror("Error on accept");
	    else{	    	
	    	cli->SetName(inet_ntoa(m_clientAddr.sin_addr));
	        thr->Create(TTracker::HandleClient, cli);
	    }
	}
}

void TTracker::HandleClient(TSocket *cli){
	char buffer[m_bits_size];
	std::string text = "";

	int n;
	cli->SetId(TTracker::m_peers.size());
	cli->m_state = true;

	std::cout << "client: " << cli->m_name << " Created\tid: " << cli->m_id << "\n";

	TTracker::m_peers.push_back(*cli);
	
	int idx;
	TProtocol mtcp(m_bits_size);
	while(cli->m_state){
		std::cout << mtcp.Receiving(cli->m_sock) << "\n";
	}
}

void TTracker::Disconnet(int idx){	
	TTracker::m_cmutex.lock();
		m_peers[idx].m_state = false;
		std::string text = "";
		send(m_peers[idx].m_sock, text.c_str(), text.size(), 0);
		std::cout << m_peers[idx].m_name << " disconneted\n";
		close(m_peers[idx].m_sock);
	
		TTracker::m_peers.erase(TTracker::m_peers.begin()+idx);
	TTracker::m_cmutex.unlock();
}

void TTracker::SendToAllClients(std::string _text){
	TTracker::m_cmutex.lock();
		for(unsigned i=0; i<m_peers.size(); i++){			
			send(TTracker::m_peers[i].m_sock, _text.c_str(), _text.size(), 0);
		}
	TTracker::m_cmutex.unlock();
}

int TTracker::FindClientIdx(TSocket *_cli){
	for(unsigned i=0; i<m_peers.size(); i++)
		if(TTracker::m_peers[i].m_id == _cli->m_id)
			return i;

	return 0;
}

TTracker::~TTracker(){
	m_peers.clear();
}

#endif
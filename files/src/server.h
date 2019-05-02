#ifndef _SERVER_H_
#define _SERVER_H_

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
#include "read-write.h"

class TServer{
private:
	static std::mutex m_qmutex;
	static std::vector<TSocket> m_clients;
	
	static std::vector<int> m_socks;
	static std::vector<int> m_ports;
	static std::vector<struct sockaddr_in> m_addrs;

	struct sockaddr_in m_serverAddrClients;

	static int m_serverSockClients;
	static int FindClientIdx(TSocket *);
	static int m_size;
public:
	TServer(int);
	~TServer();

	void ClientPort(int);
	static void HandleClient(TSocket *);
	static void ListeningClient();

	static void ConnectToClientServer(int, std::string);
};

std::vector<int> TServer::m_socks;
std::vector<int> TServer::m_ports;
std::vector<struct sockaddr_in> TServer::m_addrs;

std::vector<TSocket> TServer::m_clients;
int TServer::m_serverSockClients;
std::mutex TServer::m_qmutex;

int TServer::m_size;
TServer::TServer(int _size){
	m_serverSockClients  = socket(AF_INET, SOCK_STREAM, 0);
	m_ports = {10001, 10002, 10003};
	m_socks = std::vector<int>(3);
	m_addrs = std::vector<struct sockaddr_in>(3);

	this->m_size = _size;

	for(unsigned i=0; i<3; i++){
		m_socks[i] = socket(AF_INET, SOCK_STREAM, 0);
	}
}

void TServer::ClientPort(int _port){
	int reuse = 1;	
    memset(&m_serverAddrClients, 0, sizeof(sockaddr_in));

    m_serverAddrClients.sin_family = AF_INET;
    m_serverAddrClients.sin_addr.s_addr = INADDR_ANY;
    m_serverAddrClients.sin_port = htons(_port);

    //Avoid bind error if the socket was not close()
    setsockopt(m_serverSockClients,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));

    if(bind(m_serverSockClients, (struct sockaddr *) &m_serverAddrClients, sizeof(sockaddr_in)) < 0)
        perror("Failed to bind");

    listen(m_serverSockClients, 5);
    std::cout << "Server to Clients Created! [Listening]\n";
}

void TServer::ListeningClient(){
	TSocket	*qu;
	TThread	*thr;

	socklen_t cli_size = sizeof(sockaddr_in);
	struct sockaddr_in m_queryAddr;

	while(true){
		qu = new TSocket();
		thr = new TThread();

		qu->m_sock = accept(m_serverSockClients, (struct sockaddr *) &m_queryAddr, &cli_size);

	    if(qu->m_sock < 0)
	        perror("Error on accept");
	    else{
	        thr->Create(TServer::HandleClient, qu);
	    }
	}
}

void TServer::ConnectToClientServer(int _id, std::string _ip){
    if(m_socks[_id] < 0){
        perror("Can not Create Socket!");
        exit(0);
    }

    memset(&m_addrs[_id], 0, sizeof(m_addrs[_id]));
    m_addrs[_id].sin_family = AF_INET;
    m_addrs[_id].sin_port = htons(m_ports[_id]);
    // m_addrs[_id].sin_addr.s_addr = inet_addr(_ip.c_str());
 
    int res = inet_pton(AF_INET, _ip.c_str(), &m_addrs[_id].sin_addr);
 
    if (0 > res){
      perror("error: first parameter is not a valid address family");
      close(m_socks[_id]);
      exit(EXIT_FAILURE);
    }
    else if (0 == res){
      perror("char string (second parameter does not contain valid ipaddress");
      close(m_socks[_id]);
      exit(EXIT_FAILURE);
    }

    if (connect(m_socks[_id], (const struct sockaddr *)&m_addrs[_id], sizeof(m_addrs[_id])) < 0){
        perror("Connect failed"); 
        exit(1);
    }
}

void TServer::HandleClient(TSocket *qu){

	char buffer[m_size];
	std::string text = "", tmp;

	int idx, n;

	TServer::m_qmutex.lock();
		qu->SetId(TServer::m_clients.size());
		ConnectToClientServer(qu->m_id, "127.0.0.1");
	
		std::cout << "client connected\tid: " << qu->m_id << "\n";
		TServer::m_clients.push_back(*qu);

	TServer::m_qmutex.unlock();
	
	bool t = true;

	// srand(time(NULL));
	int rnd = rand()%100;

	while(t){
		memset(buffer, 0, sizeof(buffer));
		n = recv(qu->m_sock, buffer, sizeof(buffer), 0);

		if(n == 0){
			std::cout << qu->m_id << " id disconneted in the server\n";
			n = send(m_socks[qu->m_id], buffer, sizeof(buffer), 0);

			// std::this_thread::sleep_for(std::chrono::milliseconds(200));
			close(qu->m_sock);

			/*TServer::m_qmutex.lock();				
				idx = TServer::FindClientIdx(qu);
				TServer::m_clients.erase(TServer::m_clients.begin()+idx);
			TServer::m_qmutex.unlock();*/
			t = false;
			// break;
		}
		else if(n < 0){
			perror("error receiving text");
		}
		else{
			tmp = read_buffer(buffer, m_size);
			text = str_complete(std::to_string(stoi(tmp)+rnd), m_size);
			send(m_socks[qu->m_id], text.c_str(), text.size(), 0);
		}
	}
}

int TServer::FindClientIdx(TSocket *_q){
	for(unsigned i=0; i<m_clients.size(); i++)
		if(TServer::m_clients[i].m_id == _q->m_id)
			return i;

	return 0;
}

TServer::~TServer(){
	m_clients.clear();
}

#endif
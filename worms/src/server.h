#ifndef _SERVER_H_
#define _SERVER_H_

#include <iostream>
#include <vector>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "data.h"
#include "thread.h"

class TServer{
private:
	static std::vector<TData> m_clients;
	int m_serverSock, m_clientSock;
	struct sockaddr_in m_serverAddr, m_clientAddr;
	char buffer[256];
	
	static void SendToAll(char *);
	static int  FindClientIdx(TData *);
public:
	TServer();
	~TServer();

	void Connect(int);
	void Listening();
	static void * HandleClient(void *);
};

std::vector<TData> TServer::m_clients;

TServer::TServer(){
	TThread::InitMutex();
	m_serverSock = socket(AF_INET, SOCK_STREAM, 0);
}

void TServer::Connect(int _port){
	int reuse = 1;	
    memset(&m_serverAddr, 0, sizeof(sockaddr_in));

    m_serverAddr.sin_family = AF_INET;
    m_serverAddr.sin_addr.s_addr = INADDR_ANY;
    m_serverAddr.sin_port = htons(_port);

    //Avoid bind error if the socket was not close()
    setsockopt(m_serverSock,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));

    if(bind(m_serverSock, (struct sockaddr *) &m_serverAddr, sizeof(sockaddr_in)) < 0)
        perror("Failed to bind");

    listen(m_serverSock, 5);
    std::cout << "Server Created!\n";
}

void TServer::Listening(){
	TData   *cli;
	TThread *thr;

	socklen_t cli_size = sizeof(sockaddr_in);

	std::cout << "Server Listening!\n";
	while(true){
		cli = new TData();
		thr = new TThread();

		// block
		cli->m_sock = accept(m_serverSock, (struct sockaddr *) &m_clientAddr, &cli_size);

	    if(cli->m_sock < 0)
	        perror("Error on accept");
	    else
	        thr->Create((void *) TServer::HandleClient, cli);
	}
}

void *TServer::HandleClient(void *args){
	TData *cli = (TData *)args;

	char buffer[256-25];
	char text[256];

	int idx, n;

	TThread::LockMutex(cli->m_name);

		cli->SetId(TServer::m_clients.size());
		cli->SetName("[client "+std::to_string(cli->m_id)+"]");

		std::cout << cli->m_name << " connected\tid: " << cli->m_id << "\n";
		TServer::m_clients.push_back(*cli);

	TThread::UnlockMutex(cli->m_name);

	while(true){
		memset(buffer, 0, sizeof(buffer));
		n = recv(cli->m_sock, buffer, sizeof(buffer), 0);

		if(n == 0){
			std::cout << cli->m_name << " disconneted\n";
			close(cli->m_sock);

			TThread::LockMutex(cli->m_name);
				idx = TServer::FindClientIdx(cli);
				TServer::m_clients.erase(TServer::m_clients.begin()+idx);
			TThread::UnlockMutex(cli->m_name);
			break;
		}
		else if(n < 0){
			perror("error receiving text");
		}
		else{
			// std::cout << "text from " << cli->m_name << "\n";
			snprintf(text, sizeof text, "%s: %s", cli->m_name.c_str(), buffer); 
			TServer::SendToAll(text);
		}
	}

	return NULL;
}

void TServer::SendToAll(char *text){
	TThread::LockMutex("'Send'");
		for(unsigned i=0; i<m_clients.size(); i++)
			send(TServer::m_clients[i].m_sock, text, strlen(text), 0);
	TThread::UnlockMutex("'Send'");
}

int TServer::FindClientIdx(TData *_cli){
	for(unsigned i=0; i<m_clients.size(); i++)
		if(TServer::m_clients[i].m_id == _cli->m_id)
			return i;

	return -1;
}

TServer::~TServer(){
	m_clients.clear();
}

#endif
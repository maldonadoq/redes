#ifndef _SERVER_H_
#define _SERVER_H_

#include <iostream>
#include <vector>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "argument.h"
#include "worm.h"
#include "data.h"
#include "thread.h"
#include "utils.h"

class TServer{
private:
	static std::vector<TData> m_clients;	
	static std::vector<TWorm> m_worms;
	static std::vector<std::pair<int, int> > m_fruits;

	int m_serverSock, m_clientSock;
	struct sockaddr_in m_serverAddr, m_clientAddr;
	static unsigned m_rsize;
	static unsigned m_csize;
	static char **m_table;
	char buffer[256];

	static int  FindClientIdx(char);
	static void SendToAll(std::string);
	static int  FindClientIdx(TData *);
	static bool IsAvatarUsed(char);
	static void UpdateTable(int);
	static void ClearTable();	
	static int  IsFruit(int, int);
public:
	TServer();
	~TServer();

	void Connect(int);
	void Listening();
	void CreateTable(unsigned, unsigned);
	void GenerateFruit(unsigned);
	static void * HandleClient(void *);
};

std::vector<TData> TServer::m_clients;
std::vector<TWorm> TServer::m_worms;
std::vector<std::pair<int, int> > TServer::m_fruits;
char** TServer::m_table;

unsigned TServer::m_rsize;
unsigned TServer::m_csize;

TServer::TServer(){
	TThread::InitMutex();
	m_serverSock = socket(AF_INET, SOCK_STREAM, 0);	
}

void TServer::CreateTable(unsigned _r, unsigned _c){
	this->m_rsize = _r;
	this->m_csize = _c;

	this->m_table = new char*[m_rsize];

	unsigned i, j;

	for(i=0; i<m_rsize; i++){
		this->m_table[i] = new char[m_csize];
		for(j=0; j<m_csize; j++){
			this->m_table[i][j] = ' ';
		}
	}

	GenerateFruit(25);
}

void TServer::ClearTable(){
	unsigned i, j;

	for(i=1; i<m_rsize-1; i++){
		for(j=1; j<m_csize-1; j++){
			m_table[i][j] = ' ';
		}
	}

	for(j=0; j<m_csize; j++){
		m_table[0][j] = '-';
		m_table[m_rsize-1][j] = '-';
	}

	for(i=0; i<m_rsize; i++){
		m_table[i][0] = '|';
		m_table[i][m_csize-1] = '|';
	}
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

void TServer::GenerateFruit(unsigned _t){
	int tr, tc;

	srand(time(NULL));
	for(unsigned i=0; i<_t; i++){		
		tr = 1+(rand()%(m_rsize-2));
		tc = 1+(rand()%(m_csize-2));

		m_fruits.push_back(std::make_pair(tr,tc));
	}
}

void TServer::Listening(){
	TArg    *arg;
	TThread *thr;

	socklen_t cli_size = sizeof(sockaddr_in);

	std::cout << "Server Listening!\n";
	while(true){
		arg = new TArg();
		thr = new TThread();

		// block
		arg->m_client->m_sock = accept(m_serverSock, (struct sockaddr *) &m_clientAddr, &cli_size);

	    if(arg->m_client->m_sock < 0)
	        perror("Error on accept");
	    else
	        thr->Create((void *) TServer::HandleClient, arg);
	}
}

int TServer::IsFruit(int _r, int _c){
	for(unsigned i=0; i<m_fruits.size(); i++){
		if((m_fruits[i].first == _r) and (m_fruits[i].second == _c)){
			return (int)i;
		}
	}

	return -1;
}

void TServer::UpdateTable(int _idx){
	unsigned i, j;

	ClearTable();

	for(i=0; i<m_fruits.size(); i++)
		m_table[m_fruits[i].first][m_fruits[i].second] = '*';

	std::pair<int, int> tmp;
	for(i=0; i<m_worms.size(); i++){
		tmp = m_worms[i].m_head;
		
		for(auto pos:m_worms[i].m_body){
			m_table[pos.first][pos.second] = m_clients[i].m_avatar;
		}
		// m_table[m_worms[i].m_head.first][m_worms[i].m_head.second] = 'O';
	}

	int itm;
	int s;

	tmp = m_worms[_idx].m_head;
	itm = IsFruit(tmp.first, tmp.second);

	if(itm >= 0){
		TServer::m_fruits.erase(TServer::m_fruits.begin()+itm);
		m_worms[_idx].Grow(tmp.first, tmp.second);
	}

	for(j=0; j<m_worms.size(); j++){
		if(_idx!=j){
			if(m_worms[j].IsThere(tmp.first, tmp.second)){					
				s = m_worms[j].RemoveBody();					
				m_worms[_idx].AddQueue(s);
				std::cout << m_clients[_idx].m_avatar << " eats " << s << " tails of " << m_clients[j].m_avatar << "\n";
				break;
			}
		}
	}
}

bool TServer::IsAvatarUsed(char _a){
	for(unsigned i=0; i<m_clients.size(); i++)
		if(m_clients[i].m_avatar == _a)
			return true;

	return false;
}

void *TServer::HandleClient(void *_args){
	TArg  *arg = (TArg  *)_args;
	
	TData *cli = arg->m_client;
	TWorm *wor = arg->m_worm;

	char buffer[256-25];
	std::string text = "";

	int idx, n;

	TThread::LockMutex(cli->m_name);

		cli->SetId(TServer::m_clients.size());
		cli->SetName("[client "+std::to_string(cli->m_id)+"]");

		bool avatar = true;
		while(avatar){
			memset(buffer, 0, sizeof(buffer));
			n = recv(cli->m_sock, buffer, sizeof(buffer), 0);
			
			if(n > 0){
				if(!TServer::IsAvatarUsed(buffer[0])){
					cli->SetAvatar(buffer[0]);					
					avatar = false;

					text = "1";
					send(cli->m_sock, text.c_str(), text.size(), 0);
				}
				else{
					text = "0";
					send(cli->m_sock, text.c_str(), text.size(), 0);
				}
			}
		}

		srand(time(NULL));
		unsigned tr = 1+(rand()%(m_rsize-2));
		unsigned tc = 1+(rand()%(m_csize-2));

		wor->SetHead(tr, tc);

		std::cout << cli->m_name << " connected\tid: " << cli->m_id << "\tavatar: " << cli->m_avatar << "\n";
		TServer::m_clients.push_back(*cli);
		TServer::m_worms.push_back(*wor);		
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
				TServer::m_worms.erase(TServer::m_worms.begin()+idx);
			TThread::UnlockMutex(cli->m_name);
			break;
		}
		else if(n < 0){
			perror("error receiving text");
		}
		else{
			idx = TServer::FindClientIdx(cli);
			m_worms[idx].Move(buffer[0], m_rsize, m_csize);
			TServer::UpdateTable(idx);
			text = table_to_str(m_table, m_rsize, m_csize);
			TServer::SendToAll(text);
		}
	}

	return NULL;
}

void TServer::SendToAll(std::string text){
	TThread::LockMutex("'Send'");
		// std::cout << "\ntext sending: " << text << "\n";
		for(unsigned i=0; i<m_clients.size(); i++)
			send(TServer::m_clients[i].m_sock, text.c_str(), text.size(), 0);
	TThread::UnlockMutex("'Send'");
}

int TServer::FindClientIdx(TData *_cli){
	for(unsigned i=0; i<m_clients.size(); i++)
		if(TServer::m_clients[i].m_id == _cli->m_id)
			return i;

	return 0;
}

int TServer::FindClientIdx(char _a){
	for(unsigned i=0; i<m_clients.size(); i++)
		if(TServer::m_clients[i].m_avatar == _a)
			return i;

	return -1;
}

TServer::~TServer(){
	m_clients.clear();
}

#endif
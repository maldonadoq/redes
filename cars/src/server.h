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

#include "car.h"
#include "socket-info.h"
#include "thread-u.h"
#include "utils.h"

extern unsigned rsize;
extern unsigned csize;

class TServer{
private:
	static std::mutex m_cmutex;
	static std::vector<TSocket> m_clients;
	static std::vector<TCar> m_cars;

	struct sockaddr_in m_server_addr;
	static int m_server_sock;
	static char **m_table;

	static void SendToAllClients(std::string);
	static int  FindClientIdx(TSocket *);	
	static std::string GetLine(int);
	static bool IsAvatarUsed(char);
	static void UpdateTable(int);
	static void ClearTable(std::string);	
	static void PrintBody(int);
public:
	TServer();
	~TServer();

	void Connect(int);	
	void CreateTable();

	static void Listening();
	static void Send();
	static void HandleClient(TSocket *, TCar *);
};

std::vector<TSocket>	TServer::m_clients;
std::vector<TCar>		TServer::m_cars;
int 					TServer::m_server_sock;
std::mutex 				TServer::m_cmutex;
char** 					TServer::m_table;

TServer::TServer(){
	m_server_sock = socket(AF_INET, SOCK_STREAM, 0);
}

void TServer::CreateTable(){
	this->m_table = new char*[rsize];

	unsigned i, j;

	for(i=0; i<rsize; i++){
		this->m_table[i] = new char[csize];

		for(j=0; j<csize; j++){
			this->m_table[i][j] = ' ';
		}
	}
}

void TServer::ClearTable(std::string _line){
	unsigned i, j;

	for(i=1; i<rsize-1; i++){
		for(j=1; j<csize-1; j++){
			m_table[i][j] = ' ';
		}
	}

	for(i=0; i<rsize; i++){
		m_table[i][0] = _line[i];
		m_table[i][csize-1] = _line[i];
	}
}

void TServer::PrintBody(int _idx){
	m_table[m_cars[_idx].m_x][m_cars[_idx].m_y] = m_cars[_idx].m_avatar;
	m_table[m_cars[_idx].m_x+1][m_cars[_idx].m_y] = m_cars[_idx].m_avatar;
	m_table[m_cars[_idx].m_x+1][m_cars[_idx].m_y-1] = m_cars[_idx].m_avatar;
	m_table[m_cars[_idx].m_x+1][m_cars[_idx].m_y+1] = m_cars[_idx].m_avatar;

	m_table[m_cars[_idx].m_x-1][m_cars[_idx].m_y] = m_cars[_idx].m_avatar;
	m_table[m_cars[_idx].m_x-1][m_cars[_idx].m_y-1] = m_cars[_idx].m_avatar;
	m_table[m_cars[_idx].m_x-1][m_cars[_idx].m_y+1] = m_cars[_idx].m_avatar;
}


void TServer::UpdateTable(int _t){
	unsigned i, j;

	ClearTable(GetLine(_t));

	for(i=0; i<m_cars.size(); i++){
		if(m_cars[i].Brake(rsize)){
			PrintBody(i);
		}
	}
}

void TServer::Connect(int _port){
	int reuse = 1;	
    memset(&m_server_addr, 0, sizeof(sockaddr_in));

    m_server_addr.sin_family = AF_INET;
    m_server_addr.sin_addr.s_addr = INADDR_ANY;
    m_server_addr.sin_port = htons(_port);

    //Avoid bind error if the socket was not close()
    setsockopt(m_server_sock,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));

    if(bind(m_server_sock, (struct sockaddr *) &m_server_addr, sizeof(sockaddr_in)) < 0)
        perror("Failed to bind");

    listen(m_server_sock, 5);
    std::cout << "Server Created!\n";
    std::cout << "Listening!\n";
}

std::string TServer::GetLine(int init){
	std::string _tab = "";
	std::string _t;

	switch(init){
		case 0:
			_t = "| ";
			break;
		case 1:
			_t = " |";
			break;
		default:
			break;
	}

	for(unsigned i=0; i<rsize; i+=2){
		_tab += _t;
	}

	return _tab;
}

void TServer::Listening(){
	TSocket	*cli;
	TThread	*thr;
	TCar	*car;

	socklen_t cli_size = sizeof(sockaddr_in);
	struct sockaddr_in m_clientAddr;

	while(true){
		cli = new TSocket();
		thr = new TThread();
		car = new TCar();

		// block
		cli->m_sock = accept(m_server_sock, (struct sockaddr *) &m_clientAddr, &cli_size);

	    if(cli->m_sock < 0)
	        perror("Error on accept");
	    else{	    	
	    	cli->SetName(inet_ntoa(m_clientAddr.sin_addr));
	        thr->Create(TServer::HandleClient, cli, car);
	    }
	}
}

void TServer::HandleClient(TSocket *cli, TCar *car){

	char buffer[256];
	std::string text = "";

	int n;

	TServer::m_cmutex.lock();

		cli->SetId(TServer::m_clients.size());
	
		bool avatar = true;
		while(avatar){
			memset(buffer, 0, sizeof(buffer));
			n = recv(cli->m_sock, buffer, sizeof(buffer), 0);
			
			if(n > 0){
				if(!TServer::IsAvatarUsed(buffer[0])){
					car->SetAvatar(buffer[0]);
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
		unsigned tr = 1;
		unsigned tc = 2+(rand()%(csize-3));

		car->SetPosition(tr, tc);
		// std::cout << "[" << tr << "," << tc << "]\n";
		std::cout << "client: " << cli->m_name << " connected\tid: " << cli->m_id << "\n";

		TServer::m_clients.push_back(*cli);
		TServer::m_cars.push_back(*car);

	TServer::m_cmutex.unlock();
	
	int idx;
	bool t = true;
	while(t){
		memset(buffer, 0, sizeof(buffer));
		n = recv(cli->m_sock, buffer, sizeof(buffer), 0);

		if(n == 0){
			std::cout << cli->m_name << " disconneted\n";
			close(cli->m_sock);

			TServer::m_cmutex.lock();
				idx = TServer::FindClientIdx(cli);
				TServer::m_clients.erase(TServer::m_clients.begin()+idx);
				TServer::m_cars.erase(TServer::m_cars.begin()+idx);
			TServer::m_cmutex.unlock();
			break;
		}
		else if(n < 0){
			perror("error receiving text");
		}
		else{
			idx = TServer::FindClientIdx(cli);
			t = m_cars[idx].Move(buffer[0], rsize, csize);

			// here
			if(!t){
				text = "";

				send(cli->m_sock, text.c_str(), text.size(), 0);
				std::cout << cli->m_name << " disconneted\n";
					close(cli->m_sock);

				TServer::m_cmutex.lock();
					idx = TServer::FindClientIdx(cli);
					TServer::m_clients.erase(TServer::m_clients.begin()+idx);
					TServer::m_cars.erase(TServer::m_cars.begin()+idx);
				TServer::m_cmutex.unlock();
			}
		}
	}
}

void TServer::Send(){
	int _sleep = 400;
	int initial = 0;

	std::string text;

	while(true){
		std::this_thread::sleep_for(std::chrono::milliseconds(_sleep));
		UpdateTable(initial%2);
		text = table_to_str(m_table, rsize, csize);
		SendToAllClients(text);
		initial++;
	}
}

void TServer::SendToAllClients(std::string _text){
	TServer::m_cmutex.lock();
		for(unsigned i=0; i<m_clients.size(); i++){			
			send(TServer::m_clients[i].m_sock, _text.c_str(), _text.size(), 0);
		}
	TServer::m_cmutex.unlock();
}

int TServer::FindClientIdx(TSocket *_cli){
	for(unsigned i=0; i<m_clients.size(); i++)
		if(TServer::m_clients[i].m_id == _cli->m_id)
			return i;

	return 0;
}

bool TServer::IsAvatarUsed(char _a){
	for(unsigned i=0; i<m_cars.size(); i++)
		if(m_cars[i].m_avatar == _a)
			return true;

	return false;
}

TServer::~TServer(){
	m_clients.clear();
}

#endif
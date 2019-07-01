#ifndef _FRONT_H_
#define _FRONT_H_

#include <vector>
#include <string.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "protocol.h"
#include "utils.h"

using std::thread;
using std::string;
using std::vector;
using std::cout;
using std::cin;

using std::mutex;
using std::condition_variable;
using std::unique_lock;

mutex mtx;
condition_variable cv;

class TFront{
private:
	static int m_bits_size;
	static vector<TInfo> m_slaves;		// slaves disponible
	static TInfo m_info;				// info [port, ip]
	static TAddress m_conn;			// address [fd, addr]

	static void talking();
	static void query(string);
	static void listening();

	void init();
	void set_slaves();	
public:
	TFront();
	TFront(int, string);
	~TFront();

	void run();
};

int 			TFront::m_bits_size;
vector<TInfo>	TFront::m_slaves;
TInfo 			TFront::m_info;
TAddress 		TFront::m_conn;

TFront::TFront(int _port, string _ip){
	this->m_bits_size = 64;
	this->m_info = {_port, _ip};
	this->m_conn.sock = socket(AF_INET, SOCK_STREAM, 0);
}

void TFront::init(){
	int reuse = 1;
    memset(&m_conn.addr, 0, sizeof(sockaddr_in));

    m_conn.addr.sin_family = AF_INET;
    m_conn.addr.sin_addr.s_addr = INADDR_ANY;
    m_conn.addr.sin_port = htons(m_info.port);

    //Avoid bind error if the socket was not close()
    setsockopt(m_conn.sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));

    int nr = bind(m_conn.sock, (struct sockaddr *) &m_conn.addr, sizeof(sockaddr_in));
    if(nr < 0){
        perror("Failed to bind");
    }
    listen(m_conn.sock, 10);
}

void TFront::set_slaves(){
	/*int ns;
	cout << "Slaves Number: "; cin >> ns;

	int port;
	string ip;
	for(int i=0; i<ns; i++){
		cout << "  Slave " << i+1 << "\n";
		cout << "    Port: "; cin >> port;
		cout << "    Ip: "; cin >> ip; cout << "\n";
		m_slaves.push_back({port, ip});
	}*/

	m_slaves.push_back({8000, "127.0.0.1"});
	m_slaves.push_back({8004, "127.0.0.1"});
}

void TFront::query(string _query){
	vector<string> parse;
	string sql;

	int idx = mhash(m_slaves.size());

	bool state = true;

	switch(_query[0]){
		case 'i':
		case 'I':{
			parse = split_message(_query, " ");
			sql = "|insert into ";
			if((parse[1] == "n") or (parse[1] == "N")){
				sql += "NODE(NAME, ATTRIBUTE) VALUES ('" + parse[2] + "', '" + parse[3] +"');";
			}
			else if((parse[1] == "r") or (parse[1] == "R")){
				sql += "RELATION(NAME1, NAME2) VALUES ('" + parse[2] + "', '" + parse[3] +"');";
			}
			connect_and_send(m_slaves[idx], tinfo_to_str(m_info) + sql, "I", m_bits_size);			
			break;
		}
		case 'q':
		case 'Q':{
			parse = split_message(_query, " ");
			sql = "|select * from ";
			if((parse[1] == "n") or (parse[1] == "N")){
				sql += "NODE where NAME = '" + parse[2] + "';";
			}
			else if((parse[1] == "r") or (parse[1] == "R")){
				sql += "RELATION where NAME1 = '" + parse[2] + "';";
			}
			connect_and_send(m_slaves[idx], tinfo_to_str(m_info) + sql, "Q", m_bits_size);
			break;
		}
		case 'u':
		case 'U':{
			parse = split_message(_query, " ");
			sql = "|update NODE SET ATTRIBUTE = '" + parse[2] + "' where NAME = '" + parse[1] + "';";
			connect_and_send(m_slaves[idx], tinfo_to_str(m_info) + sql, "U", m_bits_size);
			break;
		}
		case 'd':
		case 'D':{
			parse = split_message(_query, " ");
			sql = "|delete from ";
			if((parse[1] == "n") or (parse[1] == "N")){
				sql += "NODE where NAME = '" + parse[2] + "';";
			}
			else if((parse[1] == "r") or (parse[1] == "R")){
				sql += "RELATION where NAME1 = '" + parse[2] + "' and NAME2 = '" + parse[3] + "';";
			}
			connect_and_send(m_slaves[idx], tinfo_to_str(m_info) + sql, "D", m_bits_size);
			break;
		}
		case 'a':
		case 'A':{
			parse = split_message(_query, " ");
			sql = "|select * from ";

			if((parse[1] == "n") or (parse[1] == "N")){
				sql += "NODE;";
			}
			else if((parse[1] == "r") or (parse[1] == "R")){
				sql += "RELATION;";
			}
			connect_and_send(m_slaves[idx], tinfo_to_str(m_info) + sql, "Q", m_bits_size);
			break;
		}
		default:{
			state = false;
			break;
		}
	}

	if(state){
		unique_lock<mutex> lck(mtx);
    	cv.wait(lck);
	}
}

void TFront::talking(){
	int idx;
	string comm;
	while(true){
		cout << " sarah: ";
		getline(cin, comm);		
		query(comm);		
	}
}

void TFront::listening(){
	socklen_t qsize = sizeof(sockaddr_in);
    TAddress tconn;
    string command;

    TProtocol mtcp(m_bits_size);
    TInfo tinfo;

    while(true){
        tconn.sock = accept(m_conn.sock, (struct sockaddr *) &tconn.addr, &qsize);
        if(tconn.sock < 0)
            perror("Error on accept");
        else{
            command = mtcp.receiving(tconn.sock);
            switch(command[0]){
                case 'O':
                case 'o':{
					cout << "   " << command.substr(1) << " : Ok!\n";
                    break;
                }
                case 'E':
                case 'e':{
                    cout << "   " << command.substr(1) << ": Error!\n";
                    break;
                }
                case 'Q':
                case 'q':{
                    cout << "   Query: Ok!\n";
					str_to_matrix(command.substr(1), "|", "/", "    ");
                    break;
                }
                default:
                    break;
            }
			cv.notify_all();
        }
        shutdown(tconn.sock, SHUT_RDWR);
        close(tconn.sock);
    }

    shutdown(m_conn.sock, SHUT_RDWR);
    close(m_conn.sock);
}

void TFront::run(){

	init();
	set_slaves();

	thread ttalk(talking);
    thread tlisten(listening);

    ttalk.join();
    tlisten.join();

    close(m_conn.sock);
}

TFront::~TFront(){
	m_slaves.clear();
}

#endif
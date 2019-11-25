#ifndef _FRONT_H_
#define _FRONT_H_

#include <vector>
#include <string.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <utility>
#include <mutex>
#include <condition_variable>

#include "protocol.h"
#include "utils.h"

using std::ofstream;
using std::thread;
using std::string;
using std::vector;
using std::cout;
using std::pair;
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
	static TAddress m_conn;				// address [fd, addr]
	static int m_deep;
	static int m_intermedium;
	static vector<pair<string, string> > result_deep;	// nodes intermedium

	static void talking();
	static void load_graph();
	static void query(string);
	static void listening();

	void init();
	void set_slaves();
	void load_config();
	void save_config();
public:
	TFront();
	TFront(int, string);
	~TFront();

	void run();
};

vector<pair<string, string> > 	TFront::result_deep;

int 			TFront::m_deep;
int 			TFront::m_intermedium;
int 			TFront::m_bits_size;
vector<TInfo>	TFront::m_slaves;
TInfo 			TFront::m_info;
TAddress 		TFront::m_conn;

TFront::TFront(int _port, string _ip){
	this->m_bits_size = 64;
	this->m_info = {_port, _ip};
	this->m_conn.sock = socket(AF_INET, SOCK_STREAM, 0);
	this->m_deep = 0;
	this->m_intermedium = 0;
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

	bool ce = util_exists("db/config.txt");

	if(ce){
		cout << "Loading config\n";
		load_config();
	}
	else{
		int ns;
		cout << "Slaves Number: "; cin >> ns;

		int port;
		string ip;
		for(int i=0; i<ns; i++){
			cout << "  Slave " << i+1 << "\n";
			cout << "    Port: "; cin >> port;
			cout << "    Ip: "; cin >> ip;
			cout << "\n";

			m_slaves.push_back({port, ip});
		}
		save_config();
	}

	/*m_slaves.push_back({8000, "127.0.0.1"});
	m_slaves.push_back({8004, "127.0.0.1"});*/
}

void TFront::save_config(){
	ofstream config("db/config.txt");
	int ns = (int)m_slaves.size();
	for(int i=0; i<ns; i++){
		if(i != ns-1){
			config << tinfo_to_str(m_slaves[i]) << "\n";
		}
		else{
			config << tinfo_to_str(m_slaves[i]);
		}
	}

	config.close();
}

void TFront::load_config(){
	vector<string> tconf = split_message(read_file("db/config.txt"), "\n");
	vector<string> infor;

	for(unsigned i=0; i<tconf.size(); i++){
		infor = split_message(tconf[i], "|");
		m_slaves.push_back({stoi(infor[0]), infor[1]});
	}
}

void TFront::load_graph(){
	vector<string> vsql = split_message(read_file("db/test.txt"), "\n");
	string sql;
	for(uint i=0; i<vsql.size(); i++){
		sql = vsql[i];
		cout << i << " ";
		query(sql);
	}
}

void TFront::query(string _query){
	vector<string> parse;
	string sql;

	int idx;

	bool state = true;

	if((_query == "exit") or (_query =="Exit") or (_query == "q")){
		exit(0);
	}
	else if(_query == ""){
		state = false;
	}
	else{
		parse = split_message(_query, " ");

		switch(parse.size()){
			case 2:{
				switch(_query[0]){
					case 'a':
					case 'A':{
						sql = "|select * from ";
						bool bnode = (parse[1] == "n") or (parse[1] == "N");
						bool brela = (parse[1] == "r") or (parse[1] == "R");

						if(bnode or brela){
							if(bnode)		sql += "NODE;";
							else if(brela)	sql += "RELATION;";

							if(m_slaves.size() == 0){
								cout << "   Empty";
								state = false;
							}
							else if(m_slaves.size() == 1){
								connect_and_send(m_slaves[0], tinfo_to_str(m_info) + sql, "Q", m_bits_size);
							}
							else{
								unsigned tfin = m_slaves.size()-1;
								unique_lock<mutex> lck(mtx);
								for(unsigned i=0; i<tfin; i++){
									cout << "  Node " << i << "\n";
									connect_and_send(m_slaves[i], tinfo_to_str(m_info) + sql, "Q", m_bits_size);
									cv.wait(lck);
								}

								cout << "  Node " << tfin << "\n";
								connect_and_send(m_slaves[tfin], tinfo_to_str(m_info) + sql, "Q", m_bits_size);
								cv.wait(lck);

								state = false;
							}
						}
						else{
							cout << "   Error: [A N] or [A R]\n";
							state = false;
						}
						break;
					}
					default:{
						cout << "   Error: 2 -> [A]\n";
						state = false;
						break;
					}
				}
				break;
			}
			case 3:{
				switch(_query[0]){
					case 'q':
					case 'Q':{
						idx = mhash(parse[2], m_slaves.size());
						sql = "|select * from ";
						if((parse[1] == "n") or (parse[1] == "N")){
							sql += "NODE where NAME = '" + parse[2] + "';";
							connect_and_send(m_slaves[idx], tinfo_to_str(m_info) + sql, "Q", m_bits_size);
						}
						else if((parse[1] == "r") or (parse[1] == "R")){
							sql += "RELATION where NAME1 = '" + parse[2] + "';";
							connect_and_send(m_slaves[idx], tinfo_to_str(m_info) + sql, "Q", m_bits_size);
						}
						else{
							cout << "   Error: [Q N X] or [Q R X]\n";
							state = false;
						}
						break;
					}
					case 'd':
					case 'D':{
						idx = mhash(parse[2], m_slaves.size());
						if((parse[1] == "n") or (parse[1] == "N")){
							sql = "|delete from NODE where NAME = '" + parse[2] + "';";
							connect_and_send(m_slaves[idx], tinfo_to_str(m_info) + sql, "D", m_bits_size);
							unique_lock<mutex> lck(mtx);
    						cv.wait(lck);

    						/*
								Falta elimnar las relaciones cuando eliminas un nodo!!
								Tenemos que buscar el elemento con los que se relaciona y elimnar esos 
								con el hash!
    						*/

    						sql = "|delete from RELATION where NAME1 = '" + parse[2] + "';";
							connect_and_send(m_slaves[idx], tinfo_to_str(m_info) + sql, "D", m_bits_size);
							cv.wait(lck);

							state = false;
						}
						else{
							cout << "   Error: [D N X]\n";
							state = false;
						}
						break;
					}
					default:{
						cout << "   Error: 2 -> [Q - S]\n";
						state = false;
						break;
					}
				}
				break;
			}
			case 4:{
				switch(_query[0]){
					case 'i':
					case 'I':{
						idx = mhash(parse[2], m_slaves.size());
						if((parse[1] == "n") or (parse[1] == "N")){
							sql = "|insert into NODE(NAME, ATTRIBUTE) VALUES ('" + parse[2] + "', '" + parse[3] +"');";
							connect_and_send(m_slaves[idx], tinfo_to_str(m_info) + sql, "I", m_bits_size);
						}
						else if((parse[1] == "r") or (parse[1] == "R")){
							idx = mhash(parse[2], m_slaves.size());
							sql = "|insert into RELATION(NAME1, NAME2) VALUES ('" + parse[2] + "', '" + parse[3] +"');";
							connect_and_send(m_slaves[idx], tinfo_to_str(m_info) + sql, "I", m_bits_size);
							unique_lock<mutex> lck(mtx);
    						cv.wait(lck);

							idx = mhash(parse[3], m_slaves.size());
							sql = "|insert into RELATION(NAME1, NAME2) VALUES ('" + parse[3] + "', '" + parse[2] +"');";
							connect_and_send(m_slaves[idx], tinfo_to_str(m_info) + sql, "I", m_bits_size);
							cv.wait(lck);

							state = false;
						}
						else{
							cout << "   Error: [I N X Y] or [I R X Y]\n";
							state = false;
						}
						break;
					}
					case 'q':
					case 'Q':{
						idx = mhash(parse[2], m_slaves.size());
						if((parse[1] == "r") or (parse[1] == "R")){
							m_deep = stoi(parse[3]);
							m_intermedium = 1;
							result_deep.clear();
							result_deep.push_back({"]", parse[2]});
							sql = "|select * from RELATION where NAME1 = '|" + parse[2];
							connect_and_send(m_slaves[idx], tinfo_to_str(m_info) + sql, "R", m_bits_size);
						}
						else{
							cout << "   Error: [Q R X Y]\n";
							state = false;
						}
						break;
					}
					case 'u':
					case 'U':{
						idx = mhash(parse[2], m_slaves.size());
						if((parse[1] == "n") or (parse[1] == "N")){
							sql = "|update NODE SET ATTRIBUTE = '" + parse[3] + "' where NAME = '" + parse[2] + "';";
							connect_and_send(m_slaves[idx], tinfo_to_str(m_info) + sql, "U", m_bits_size);
						}
						else{
							cout << "   Error: [U N X Y]\n";
							state = false;
						}
						break;
					}
					case 'd':
					case 'D':{
						if((parse[1] == "r") or (parse[1] == "R")){
							idx = mhash(parse[2], m_slaves.size());
							sql = "|delete from RELATION where NAME1 = '" + parse[2] + "' and NAME2 = '" + parse[3] + "';";
							connect_and_send(m_slaves[idx], tinfo_to_str(m_info) + sql, "D", m_bits_size);
							unique_lock<mutex> lck(mtx);
    						cv.wait(lck);

							idx = mhash(parse[3], m_slaves.size());
							sql = "|delete from RELATION where NAME1 = '" + parse[3] + "' and NAME2 = '" + parse[2] + "';";
							connect_and_send(m_slaves[idx], tinfo_to_str(m_info) + sql, "D", m_bits_size);
							cv.wait(lck);

							state = false;

						}
						else{
							cout << "   Error: [D R X X]\n";
							state = false;
						}
						break;
					}
					default:{
						cout << "   Error: 4 -> [I - Q - U - D]\n";
						state = false;
						break;
					}
				}
				break;
			}
			case 5:{
				switch(_query[0]){
					case 'u':
					case 'U':{
						idx = mhash(parse[2], m_slaves.size());
						if((parse[1] == "r") or (parse[1] == "R")){
							sql = "|update RELATION SET NAME2 = '" + parse[4] + "' where NAME1 = '" + parse[2] + "' and NAME2 = '" + parse[3] + "';";
							connect_and_send(m_slaves[idx], tinfo_to_str(m_info) + sql, "U", m_bits_size);
						}
						else{
							cout << "   Error: [U R X Y Z]\n";
							state = false;
						}
						break;
					}
					default:{
						cout << "   Error: 1 -> [U]\n";
						state = false;
						break;
					}
				}
				break;
			}
			default:{
				cout << "   Error: Query Structure [X X] - [X X X] - [X X X X] - [X X X X X]\n";
				state = false;
				break;
			}
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
	comm = "";
	while(true){
		cout << " query: ";
		getline(cin, comm);

		if(comm == "load"){
			load_graph();
		}
		else{
			query(comm);
		}
	}
}

void TFront::listening(){
	socklen_t qsize = sizeof(sockaddr_in);
    TAddress tconn;
    string command, sql;
    vector<string> inter_nodes;
    vector<pair<int, string> > inter_sql;

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
					cv.notify_all();
                    break;
                }
                case 'E':
                case 'e':{
                    cout << "   " << command.substr(1) << " : Error!\n";
                    cv.notify_all();
                    break;
                }
                case 'Q':
                case 'q':{
                    cout << "   Query: Ok!\n";
					str_to_matrix(command.substr(1), "|", "/", "    ");
					cv.notify_all();
                    break;
                }
                case 'R':
                case 'r':{
                	m_intermedium--;
                	add_str_to_list(result_deep, inter_nodes, command.substr(1), "|", "/");

                	if(m_intermedium <= 0){
                		m_deep--;
                		if(m_deep > 0){
                			if(inter_nodes.size() == 0){
                				m_intermedium = -1;
                				m_deep = -1;
                				str_to_list(result_deep, "   ");
                				cv.notify_all();
                			}
                			else{
                				preprocesing(inter_nodes, inter_sql, m_slaves.size());
                				m_intermedium = inter_sql.size();
                				for(unsigned i=0; i<inter_sql.size(); i++){
                					sql = "|select * from RELATION where NAME1 = '" + inter_sql[i].second;
									connect_and_send(m_slaves[inter_sql[i].first], tinfo_to_str(m_info) + sql, "R", m_bits_size);
                				}
                			}
                			inter_nodes.clear();
                		}
                		else{
                			str_to_list(result_deep, "   ");
                			cv.notify_all();
                			inter_nodes.clear();
                		}
                	}
					// str_to_list(command.substr(1), "|", "    ");
                    break;
                }
                default:
                    break;
            }
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
#ifndef _SLAVE_H_
#define _SLAVE_H_

#include <string.h>
#include <iostream>

#include "database.h"
#include "protocol.h"
#include "utils.h"

using std::string;
using std::cout;

class TSlave{
private:
	int m_bits_size;
	TInfo m_info;				// info [port, ip]
	TAddress m_conn;			// address [fd, addr]
    TDatabase *m_db;            // SQLite Data Base

	void listening();
	void init();
public:
	TSlave();
	TSlave(int, string, string);
	~TSlave();

	void run();
};

TSlave::TSlave(){
	
}

TSlave::TSlave(int _port, string _ip, string _dbname){
	this->m_bits_size = 64;
	this->m_info = {_port, _ip};
	this->m_conn.sock = socket(AF_INET, SOCK_STREAM, 0);
    this->m_db = new TDatabase(_dbname);
}

void TSlave::init(){
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

void TSlave::listening(){
    socklen_t qsize = sizeof(sockaddr_in);
    TAddress tconn;
    string command, sql;

    vector<string> vparse;
    vector<vector<string> > sresult;
    TInfo tinfo;
    TProtocol mtcp(m_bits_size);    

    while(true){
        tconn.sock = accept(m_conn.sock, (struct sockaddr *) &tconn.addr, &qsize);
        if(tconn.sock < 0)
            perror("Error on accept");
        else{           
            // printf("address: %s\n", inet_ntoa(tconn.addr.sin_addr));
            // printf("port %d\n", ntohs(tconn.addr.sin_port));
            command = mtcp.receiving(tconn.sock);
            // cout << command << "\n";
            switch(command[0]){
                case 'I':
                case 'i':{
                    vparse = split_message(command.substr(1), "|");
	    			make_tinfo(vparse, tinfo, sql);
                    if(m_db->execute(sql))
                        connect_and_send(tinfo, "Create", "O", m_bits_size);
                    else
                        connect_and_send(tinfo, "Create", "E", m_bits_size);

                    break;
                }
                case 'Q':
                case 'q':{
                    vparse = split_message(command.substr(1), "|");
                    make_tinfo(vparse, tinfo, sql);
                    if(m_db->execute(sql, sresult)){
                        connect_and_send(tinfo, matrix_to_str(sresult, "|", "/"), "q", m_bits_size);
                    }
                    else
                        connect_and_send(tinfo, "Query", "E", m_bits_size);
                    
                    break;
                }
                case 'U':
                case 'u':{
                    vparse = split_message(command.substr(1), "|");
                    make_tinfo(vparse, tinfo, sql);
                    if(m_db->execute(sql))
                        connect_and_send(tinfo, "Update", "O", m_bits_size);
                    else
                        connect_and_send(tinfo, "Update", "E", m_bits_size);
                    
                    break;
                }
                case 'D':
                case 'd':{
                    vparse = split_message(command.substr(1), "|");
                    make_tinfo(vparse, tinfo, sql);
                    if(m_db->execute(sql))
                        connect_and_send(tinfo, "Delete", "O", m_bits_size);
                    else
                        connect_and_send(tinfo, "Delete", "E", m_bits_size);
                    
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

void TSlave::run(){
	init();
	listening();
}

TSlave::~TSlave(){

}

#endif
#ifndef _UTILS_H_
#define _UTILS_H_

#include <boost/algorithm/string.hpp> 
#include <netinet/in.h>

#include "protocol.h"

using std::string;
using std::cout;

struct TInfo{
	int port;		// port
	string ip;		// ip address
};

string tinfo_to_str(TInfo _info){
	return to_string(_info.port) + "|" + _info.ip;
}

struct TAddress{
	int sock;					// server sock
    struct sockaddr_in addr;	// server address
};

void connect_and_send(TInfo &_machine, string _message,
	string _type, int _bits_size){

    int msock = socket(AF_INET, SOCK_STREAM, 0);
    if(msock < 0){
        perror("Can't Create Socket!");
        exit(0);
    }

    struct sockaddr_in maddr;

    memset(&maddr, 0, sizeof(maddr));
    maddr.sin_family = AF_INET;
    maddr.sin_port = htons(_machine.port);
 
    int nr = inet_pton(AF_INET, _machine.ip.c_str(), &maddr.sin_addr);
 
    if(nr <= 0){
        perror("error: Not a valid address family");
        close(msock);
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

    nr = connect(msock, (const struct sockaddr *)&maddr, sizeof(struct sockaddr_in));
    if(nr >= 0){
        TProtocol mtcp(_bits_size);
        mtcp.sending(_message, msock, _type);
    }

    shutdown(msock, SHUT_RDWR);
    close(msock);
}

void make_tinfo(vector<string> &_parse, TInfo &_info, string &_sql){
    if(_parse.size() == 3){
        _info.port  = stoi(_parse[0]);
        _info.ip    = _parse[1];
        _sql        = _parse[2];
    }
    else{
        cout << "You Need Port, IP and SQL\n";
    }
}

vector<string> split_message(string _message, string _separator){
    vector<string> result; 
    boost::split(result, _message, boost::is_any_of(_separator));

    return result;
}

int mhash(int t){
    return (rand()%t);
}

void print_matrix(vector<vector<string> > &_mat){
    for(unsigned i=0; i<_mat.size(); i++){
        cout << "  ";
        for(unsigned j=0; j<_mat[i].size(); j++){
            cout << _mat[i][j] << " ";
        }
        cout << "\n";
    }
}

#endif
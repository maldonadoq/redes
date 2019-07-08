#ifndef _UTILS_H_
#define _UTILS_H_

#include <boost/algorithm/string.hpp> 
#include <netinet/in.h>
#include <utility>

#include "protocol.h"

using std::string;
using std::pair;
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

void make_tinfo(vector<string> &_parse, TInfo &_info, string &_sql, vector<string> &_node){
    if(_parse.size() > 3){
        _info.port  = stoi(_parse[0]);
        _info.ip    = _parse[1];
        _sql        = _parse[2];

        _node.clear();
        for(unsigned i=3; i<_parse.size(); i++){
            _node.push_back(_parse[i]);
        }
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

int mhash(string _str, int t){
    int thash = 0;

    for(char c: _str){
        thash += c;
    }

    return thash%t;
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

string matrix_to_str(vector<vector<string> > &_mat, string _sr, string _sc){
    string all = "";
    string sub = "";

    for(unsigned i=0; i<_mat.size(); i++){
        sub = "";
        for(unsigned j=0; j<_mat[i].size(); j++){
            sub += _sc + _mat[i][j];
        }
        
        all += _sr + sub.substr(1);
    }

    if(all == ""){
        all = _sr + "Nothing";
    }

    return all.substr(1);
}

string matrix_to_str(vector<vector<string> > &_mat, string _sr, int _idx){
    string all = "";

    if(_mat.size() > 1){
        for(unsigned i=1; i<_mat.size(); i++){
            all += _sr + _mat[i][_idx];
        }
    }
    else{
        return "Nothing";
    }

    return all.substr(1);
}

string list_to_str(vector<string> &_list, string _sr){
    string all = "";

    if(_list.size() > 0){
        for(unsigned i=0; i<_list.size(); i++){
            all += _sr + _list[i];
        }
    }
    else{
        return "Nothing";
    }

    return all.substr(1);
}

void str_to_matrix(string _mat, string _sr, string _sc, string _iden){
    vector<string> rows = split_message(_mat, _sr);
    vector<string> cols;

    for(unsigned i=0; i<rows.size(); i++){
        cols = split_message(rows[i], _sc);
        cout << _iden;
        for(unsigned j=0; j<cols.size(); j++){
            cout << cols[j] << " ";
        }
        cout << "\n";
    }
}

void str_to_list(string _mat, string _sr, string _iden){
    vector<string> rows = split_message(_mat, _sr);

    for(unsigned i=0; i<rows.size(); i++){
        cout << _iden << rows[i] << "\n";
    }
}

void str_to_list(vector<pair<string, string> > rows, string _iden){
    if(rows.size() > 1){
        for(unsigned i=1; i<rows.size(); i++){
            cout << _iden << rows[i].first << " -> " << rows[i].second << "\n";
        }
    }
    else{
        cout << _iden << "Empty!\n";
    }
}

vector<string> str_to_list(string _mat, string _sr){
    vector<string> rows = split_message(_mat, _sr);

    if(rows[0] == "Nothing"){
        rows.clear();
    }

    return rows;
}

void add_str_to_list(vector<pair<string, string> > &_result, vector<string> &_inter, string _mat, string _sr, string _sc){
    vector<string> rows = str_to_list(_mat, _sr);
    vector<string> cols;

    bool t;
    for(unsigned i=0; i<rows.size(); i++){
        cols = split_message(rows[i], _sc);
        t = false;

        for(unsigned j=0; j<_result.size(); j++){
            if(cols[1] == _result[j].second){
                t = true;
                break;
            }
        }

        if(!t){
            _inter.push_back(cols[1]);
        }

        _result.push_back({cols[0], cols[1]});
    }
}

void preprocesing(vector<string> &_inter_node, vector<pair<int, string> > &_inter_sql, int _th){
    _inter_sql.clear();
    vector<string> tmp;

    int i;
    for(i=0; i<_th; i++){
        tmp.push_back("");
    }

    int idx;
    for(i=0; i<(int)_inter_node.size(); i++){
        idx = mhash(_inter_node[i], _th);
        tmp[idx] += ("|" + _inter_node[i]);
    }

    for(i=0; i<(int)tmp.size(); i++){
        if(tmp[i] != ""){
            _inter_sql.push_back({i, tmp[i]});
        }
    }
}

#endif
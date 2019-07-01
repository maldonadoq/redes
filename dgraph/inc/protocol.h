#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <vector>
#include <arpa/inet.h>
#include <unistd.h>

/*
	1 -> commando				--|
	1 -> bool [body or head]	--|
	3 -> size of message		--|	size = 4
*/

using std::vector;
using std::string;
using std::to_string;

string add_zeros_left(int _number, unsigned _size){
	string tmp = to_string(_number);
	for(unsigned i=tmp.size(); i<_size; i++){
		tmp = '0'+tmp;
	}

	return tmp;
}

class TProtocol{
private:
	int m_bits_size;	// buffer size
	int m_bit;			/* example:
							1-9     -> 1
							10-99   -> 2
							100-999 -> 3
						*/

	vector<string> split_text(string);
public:
	TProtocol();
	TProtocol(int);
	~TProtocol();

	void sending(string, int, string);
	string receiving(int);
};

TProtocol::TProtocol(int _size){
	this->m_bits_size = _size;
	this->m_bit = to_string(m_bits_size).size();	
}

TProtocol::TProtocol(){

}

vector<string> TProtocol::split_text(string _text){
	unsigned _size = m_bits_size - (m_bit + 1);		// size of message
	vector<string> vsplit;							// vector os mssg

	string mssg;
	for(unsigned i=0; i<_text.size(); i+=_size){
		mssg = "";
		for(unsigned j=0; j<_size and (i + j)<_text.size(); j++){
			mssg += _text[i+j];
		}
		vsplit.push_back(mssg);
	}

	return vsplit;
}

void TProtocol::sending(string _text, int _socket, string _type){
	string _text_tmp = _type + _text;

	vector<string> messages = split_text(_text_tmp);
	string mssg;

	unsigned tmps = messages.size();

	int n;
	int buffer_ok = 2;
	char buffer[buffer_ok];
	if(tmps > 1){
		for(unsigned i=0; i<tmps-1; i++){
			mssg = "0" + add_zeros_left(messages[i].size(), m_bit) + messages[i];
			send(_socket, mssg.c_str(), mssg.size(), 0);
		}
	}

	mssg =  "1" + add_zeros_left(messages[tmps-1].size(), m_bit) + messages[tmps-1];
	send(_socket, mssg.c_str(), mssg.size(), 0);
}

string TProtocol::receiving(int _socket){
	string text = "", tmp = "";

	char buffer[m_bits_size];

	int n;
	while(true){
		memset(&buffer, 0, m_bits_size);
        n = recv(_socket, buffer, m_bits_size, 0);

        if(n > (m_bit+1)){
        	if(buffer[0] == '0'){
        		text += string(buffer).substr(m_bit+1, m_bits_size - (m_bit + 1));
        	}
        	else if(buffer[0] == '1'){
        		tmp = string(buffer).substr(1,m_bit);
        		text += string(buffer).substr(m_bit+1, stoi(tmp));
        		break;
        	}
        }
	}

	return text;
}

TProtocol::~TProtocol(){

}

#endif
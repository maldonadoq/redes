#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "utils.h"

/*
	1 -> commando				--|
	1 -> bool [body or head]	--|
	3 -> size of message		--|	size = 4
*/

class TProtocol{
private:
	static int m_bits_size;		// buffer size
	static int m_bit;			/* example:
									1-9     -> 1
									10-99   -> 2
									100-999 -> 3
								*/

	static std::vector<std::string> SplitText(std::string);
public:
	TProtocol();
	TProtocol(int);
	~TProtocol();

	static void Sending(std::string, int, std::string);
	static std::string Receiving(int);
};

int TProtocol::m_bit;
int TProtocol::m_bits_size;

TProtocol::TProtocol(int _size){
	this->m_bits_size = _size;
	this->m_bit = std::to_string(m_bits_size).size();	
}

TProtocol::TProtocol(){

}

std::vector<std::string> TProtocol::SplitText(std::string _text){
	unsigned _size = m_bits_size - (m_bit + 1);		// size of message
	std::vector<std::string> vsplit;				// vector os mssg

	std::string mssg;
	for(unsigned i=0; i<_text.size(); i+=_size){
		mssg = "";
		for(unsigned j=0; j<_size and (i + j)<_text.size(); j++){
			mssg += _text[i+j];
		}
		vsplit.push_back(mssg);
	}

	return vsplit;
}

void TProtocol::Sending(std::string _text, int _socket, std::string _type){
	std::string _text_tmp = _type + _text;

	std::vector<std::string> messages = SplitText(_text_tmp);
	std::string mssg;

	unsigned tmps = messages.size();

	int n;
	int buffer_ok = 2;
	char buffer[buffer_ok];
	if(tmps > 1){
		for(unsigned i=0; i<tmps-1; i++){
			mssg = "0" + add_zeros_left(messages[i].size(), m_bit) + messages[i];
			// std::cout << mssg << "\n";

			send(_socket, mssg.c_str(), mssg.size(), 0);
			/*memset(&buffer, 0, buffer_ok);
        	n = recv(_socket, buffer, buffer_ok, 0);

        	if(n > 0){
        		std::cout << buffer << " ";
        	}*/
		}
	}

	mssg =  "1" + add_zeros_left(messages[tmps-1].size(), m_bit) + messages[tmps-1];
	// std::cout << mssg << "\n\n";

	send(_socket, mssg.c_str(), mssg.size(), 0);
	/*memset(&buffer, 0, buffer_ok);
	n = recv(_socket, buffer, buffer_ok, 0);
	std::cout << buffer << "\n";*/
}

std::string TProtocol::Receiving(int _socket){
	std::string text = "", tmp = "";

	char buffer[m_bits_size];

	int n;
	while(true){
		memset(&buffer, 0, m_bits_size);
        n = recv(_socket, buffer, m_bits_size, 0);

        if(n > (m_bit+1)){
        	if(buffer[0] == '0'){
        		text += std::string(buffer).substr(m_bit+1, m_bits_size - (m_bit + 1));
        	}
        	else if(buffer[0] == '1'){
        		tmp = std::string(buffer).substr(1,m_bit);
				// std::cout << "size: " << stoi(tmp) << "\n";
        		text += std::string(buffer).substr(m_bit+1, stoi(tmp));
        		break;
        	}
        }
        // send(_socket, "ok", 2, 0);
	}

	return text;
}

TProtocol::~TProtocol(){

}

#endif
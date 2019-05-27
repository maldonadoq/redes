#ifndef _UTILS_H_
#define _UTILS_H_

#include <vector>
#include <termios.h>
#include <stdio.h>

// ip
#include <sys/types.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <string.h>

// split
#include <boost/algorithm/string.hpp> 

#include "peer-info.h"

using std::cout;
using std::string;
using std::vector;
using std::to_string;

static struct termios told, tnew;

void print_vector(vector<string> _vec){
	for(unsigned i=0; i<_vec.size(); i++){
		cout << _vec[i] << "\n";
	}
}

string add_zeros_left(int _number, unsigned _size){
	string tmp = to_string(_number);
	for(unsigned i=tmp.size(); i<_size; i++){
		tmp = '0'+tmp;
	}

	return tmp;
}

void initTermios() {
	tcgetattr(0, &told);
	tnew = told;
	tnew.c_lflag &= ~ICANON;
	tnew.c_lflag &= ~ECHO;
	tcsetattr(0, TCSANOW, &tnew);
}

void resetTermios() {
	tcsetattr(0, TCSANOW, &told);
}

char getch(){
	char ch;
	initTermios();

	ch = getchar();

	resetTermios();
	return ch;
}

vector<string> SplitMessage(string _message, string _separator){
    vector<string> result; 
    boost::split(result, _message, boost::is_any_of(_separator));

    return result;
}

string getIPAddress(){
    string ipAddress="Unable to get IP Address";
    struct ifaddrs *interfaces = NULL;
    struct ifaddrs *temp_addr = NULL;
    int success = 0;
    // retrieve the current interfaces - returns 0 on success
    success = getifaddrs(&interfaces);
    if (success == 0) {
        // Loop through linked list of interfaces
        temp_addr = interfaces;
        while(temp_addr != NULL) {
            if(temp_addr->ifa_addr->sa_family == AF_INET) {
                // Check if interface is en0 which is the wifi connection on the iPhone
                if(strcmp(temp_addr->ifa_name, "en0")){
                    ipAddress=inet_ntoa(((struct sockaddr_in*)temp_addr->ifa_addr)->sin_addr);
                }
            }
            temp_addr = temp_addr->ifa_next;
        }
    }
    // Free memory
    freeifaddrs(interfaces);
    return ipAddress;
}

TPeerInfo MakePeerInfo(vector<string> _parse){
    if(_parse.size() == 2){
        TPeerInfo pinfo = {_parse[0], stoi(_parse[1])};
        return pinfo;
    }
    else{
        cout << "You Need IP and Port\n";
    }

    return {"error", -1};
}

#endif
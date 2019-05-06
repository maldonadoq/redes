#include <iostream>
#include "src/client.h"

unsigned rsize = 38;
unsigned csize = 31;

// g++ -std=c++11 -pthread client.cpp -o client.out
int main(int argc, char const *argv[]){
	// std::string ip = "192.168.163.179";
	std::string ip = "127.0.0.1";
	int port = 7777;

	TClient *cl = new TClient();
	cl->Connect(ip, port);
	cl->Talking();

	delete cl;
	return 0;
}
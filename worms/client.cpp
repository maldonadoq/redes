#include <iostream>
#include "src/client.h"

unsigned rsize = 10;
unsigned csize = 50;

// build g++ -std=c++11 -pthread client.cpp -o client.out
int main(int argc, char const *argv[]){
		
	// std::string ip = "192.168.163.179";
	std::string ip = "127.0.0.1";
	int port = 8888;
	int speed = 150;

	TClient *cl = new TClient();
	cl->Connect(ip, port);
	cl->Talking(speed);

	delete cl;
	return 0;
}
#include <iostream>
#include "inc/peer.h"

// build: g++ peer.cpp -o peer.out -pthread
int main(int argc, char const *argv[]){
	std::string ip = "127.0.0.1";
	int port = 7777;
	int bs = 16;

	TPeer *cl = new TPeer(ip, port, bs);

	cl->Execute();

	delete cl;
	return 0;
}
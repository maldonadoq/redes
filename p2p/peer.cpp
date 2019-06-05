#include <iostream>
#include "inc/peer.h"

// build: g++ peer.cpp -o peer.out -pthread
int main(int argc, char const *argv[]){
	std::string ip = "127.0.0.1";
	int port = 7777;
	int bs = 64;

	TPeer *cl;

	if(argc == 2){
		cl = new TPeer(ip, port, bs, argv[1]);
	}
	else{
		cl = new TPeer(ip, port, bs);
	}

	cl->Execute();

	delete cl;
	return 0;
}
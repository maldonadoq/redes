#include <iostream>
#include "inc/peer.h"

// build: g++ peer.cpp -o peer.out -pthread
int main(int argc, char const *argv[]){
	std::string ip = "127.0.0.1";
	int port = 7777;
	int buffer_size = 16;

	TPeer *cl = new TPeer(buffer_size);

	cl->LogIn(ip, port);
	cl->Run();

	delete cl;
	return 0;
}
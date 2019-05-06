#include <iostream>
#include "src/server.h"

unsigned rsize = 38;
unsigned csize = 31;

// g++ -std=c++11 -pthread server.cpp -o server.out
int main(int argc, char const *argv[]){
	int port = 7777;		// port to clients
	TServer *s = new TServer();

	s->CreateTable();
	s->Connect(port);

	std::thread tlistening(TServer::Listening);
	std::thread tsending(TServer::Send);

	tlistening.join();

	delete s;
	return 0;
}
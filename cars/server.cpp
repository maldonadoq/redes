#include <iostream>
#include "src/server.h"

unsigned rsize = 30;
unsigned csize = 31;

// g++ -std=c++11 -pthread server.cpp -o server.out
int main(int argc, char const *argv[]){
	int port = 8888;		// port to clients
	TServer *s = new TServer();

	s->Connect(port);
	s->Listening();

	delete s;
	return 0;
}
#include <iostream>
#include "src/server.h"

int main(int argc, char const *argv[]){
	int port = 8888;
	unsigned rsize = 10;
	unsigned csize = 50;

	TServer *s = new TServer();

	s->Connect(port);
	s->CreateTable(rsize, csize);

	s->Listening();		// loop

	delete s;
	return 0;
}
#include <iostream>
#include "src/server.h"

int main(int argc, char const *argv[]){
	int port = 8888;

	TServer *s = new TServer();

	s->Connect(port);
	s->Listening();

	delete s;
	return 0;
}
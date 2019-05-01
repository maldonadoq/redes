#include <iostream>
#include "src/server.h"

int main(int argc, char const *argv[]){
	int cport = 3210;
	TServer *s = new TServer(4);

	s->ClientPort(cport);
	std::thread client(TServer::ListeningClient);

	client.join();

	delete s;
	return 0;
}
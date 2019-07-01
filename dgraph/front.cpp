#include <iostream>
#include "inc/front.h"

using std::stoi;
using std::string;

// build: g++ front.cpp -o front.out -pthread
// execu: ./front.out port ip
int main(int argc, char const *argv[]){

	int port = 6666;
	string ip = "127.0.0.1";

	if(argc == 3){
		port = stoi(argv[1]);
		ip = argv[2];
	}

	TFront *cl = new TFront(port, ip);

	cl->run();

	delete cl;
	return 0;
}
#include <iostream>
#include "src/client.h"

unsigned rsize = 10;
unsigned csize = 50;

int main(int argc, char const *argv[]){
		
	std::string ip = "192.168.1.41";	
	int port = 8888;
	int speed = 750;

	TClient *cl = new TClient();
	cl->Connect(ip, port);
	cl->Talking(speed);

	delete cl;
	return 0;
}
#include <iostream>
#include <thread>
#include "src/client.h"

int main(int argc, char const *argv[]){
	if(argc < 2){
		std::cout << "you need to put port ./client 10001\n";
	}
	else{
		std::string ip = "127.0.0.1";
		int sport = atoi(argv[1]);
		int cport = 3210;
		int nloop = 100;

		TClient *q = new TClient(4);
		q->ConnectToServer(ip, cport);
		q->CreateServer(sport);

		std::thread talk(TClient::TalkingToServer, nloop);
		std::thread listen(TClient::ListeningServer);

		talk.join();
		listen.join();

		delete q;
	}

	return 0;
}
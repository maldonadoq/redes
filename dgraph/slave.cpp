#include <iostream>
#include "inc/slave.h"

using std::stoi;
using std::string;

// build: g++ slave.cpp -o slave.out -pthread
// execu: ./slave.out port ip dbname
int main(int argc, char const *argv[]){
	int port = 8000;
	string ip = "127.0.0.1";
	string db = "db/slave.db";

	if(argc == 4){
		port = stoi(argv[1]);
		ip = argv[2];
		db = "db/" + string(argv[3]);
	}

	TSlave *sr = new TSlave(port, ip, db);

	sr->run();

	delete sr;
	return 0;
}
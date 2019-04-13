#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include "src/hash.h"

int main(int argc, char const *argv[]){
	struct sockaddr_in stSockAddr;								// Address Socket
	int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	// File Descriptor	
    int n;							// Current Buffer Lenght

    if(-1 == SocketFD){
    	perror("Can not create socket");
    	exit(1);
    }

    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
 
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(1055);
    stSockAddr.sin_addr.s_addr = INADDR_ANY;
	
	// Validate bind socket address
    if(-1 == bind(SocketFD,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in))){
		perror("Error bind failed");
		close(SocketFD);
		exit(1);
    }
 
 	// Validate if the socket is listening
    if(-1 == listen(SocketFD, 10)){
		perror("Error listen failed");
		close(SocketFD);
		exit(1);
    }

    int ConnectFD = accept(SocketFD, NULL, NULL);
	if(0 > ConnectFD){
    	perror("Error accept failed");
    	close(SocketFD);
    	exit(0);
	}

 	std::string text = "";
 	std::cout << "Server Created\n";

 	std::string msg;
 	std::string mh;

    char buffer[BufferSize];		// Create Buffer

 	while(true){
 		bzero(buffer,BufferSize);					// put zero to the buffer
		n   = read(ConnectFD, buffer, BufferSize);	// read msg in the buffer
		msg = read_buffer(buffer, 0, BufferSize);

		mh = my_hash(msg);
		n = write(ConnectFD, mh.c_str(), mh.size());
		std::cout << "hash in the server: " << mh << "\n";
 	}

    close(ConnectFD);
    close(SocketFD);

	return 0;
}
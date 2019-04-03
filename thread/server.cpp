#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include "src/read-write.h"

int main(int argc, char const *argv[]){
	struct sockaddr_in stSockAddr;								// Address Socket
	int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	// File Descriptor

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

    std::string text = "";
 	std::cout << "Server Created\n";
    while(true){
    	int ClientFD = accept(SocketFD, NULL, NULL);
		if(0 > ClientFD){
	    	perror("Error accept failed");
	    	close(SocketFD);
	    	exit(0);
		}
		else{
			std::thread tread(&thread_read, ClientFD);
			std::thread twrite(&thread_write, ClientFD);			
		}
    }
    close(SocketFD);

	return 0;
}
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
    struct sockaddr_in stSockAddr;
    int Res;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    int n;
 
    if (-1 == SocketFD){
        perror("Can not create socket");
        exit(1);
    }
 
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
 
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(1055);
    Res = inet_pton(AF_INET, "127.0.0.1", &stSockAddr.sin_addr);
 
    if (0 > Res){
        perror("Error: first parameter is not a valid address family");
        close(SocketFD);
        exit(1);
    }
    else if (0 == Res){
        perror("Char string (second parameter does not contain valid ipaddress");
        close(SocketFD);
        exit(1);
    }
 
    if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in))){
        perror("Connect failed");
        close(SocketFD);
        exit(1);
    }

    std::cout << "Client Connected\n";

    char buffer[N];
    std::string msg = read_file("data/data.txt");
    std::vector<std::string> msgs = partition(msg, BufferSize);

    std::string mh;
    std::string text;

    for(unsigned i=0; i<msgs.size(); i++){
        bzero(buffer,N);
        text = msgs[i];

        mh = my_hash(text);
        std::cout << "hash in the client: " << mh << "\t";

        n = write(SocketFD, text.c_str(), text.size());
        n = read(SocketFD, buffer, N);

        if(std::string(buffer) == mh){
            printf("%s: sending ok\n", text.c_str());
        }
        else{
            printf("%s: sending not\n", text.c_str());
        }
    }
    
    close(SocketFD);
    return 0;
  }

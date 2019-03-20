#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char const *argv[]){
    struct sockaddr_in stSockAddr;
    int Res;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    int n;
 
    if (-1 == SocketFD){
      perror("cannot create socket");
      exit(EXIT_FAILURE);
    }
 
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
 
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(5555);
    Res = inet_pton(AF_INET, "127.0.0.1", &stSockAddr.sin_addr);
 
    if (0 > Res){
      perror("error: first parameter is not a valid address family");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
    else if (0 == Res){
      perror("char string (second parameter does not contain valid ipaddress");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
 
    if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in))){
      perror("connect failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }

    n = write(SocketFD,"Hi, this is Jesamin.",20);
    
    // Read secret key send from server
    /*
    char buffer[256];
    n = read(SocketFD,buffer,255);
    printf("Here is the message: [%s]\n",buffer);
    */    
 
    shutdown(SocketFD, SHUT_RDWR);
 
    close(SocketFD);
    return 0;
  }

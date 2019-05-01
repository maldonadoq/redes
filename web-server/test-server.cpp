#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <boost/algorithm/string.hpp> 

/*
std::string dmain   = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 79\n\n<p1>Maldonado</p1><a href=\"/c1\"> Child One</a></br><a href=\"/c2\"> Child Two</a>";
std::string dchild1 = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 74\n\n<p1>Maldonado</p1><a href=\"/main\"> Main</a></br><a href=\"/c2\"> Child Two</a>";
std::string dchild2 = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 74\n\n<p1>Maldonado</p1><a href=\"/main\"> Main</a></br><a href=\"/c1\"> Child One</a>";
std::string nothing = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 34\n\n<p1>Maldonado Nothing to Do!!</p1>";
*/

std::string dmain   = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 20\n\n<p1>Main Server</p1>";
std::string dchild1 = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 18\n\n<p1>Child One</p1>";
std::string dchild2 = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 18\n\n<p1>Child Two</p1>";
std::string nothing = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 34\n\n<p1>Maldonado Nothing to Do!!</p1>";

std::string text;

void set_direction(std::string _recv){
    std::vector<std::string> result;
    boost::split(result, _recv, boost::is_any_of(" ")); 

    std::cout << "read: " << result[1] << "\n\n";

    if(result[0] == "/main"){
        text = dmain;
    }
    else if(result[0] == "/c1"){
        text = dchild1;
    }
    else if(result[0] == "/c2"){
        text = dchild2;
    }
    else{
        text = nothing;
    }
}

int main(int argc, char const *argv[]){
    struct sockaddr_in stSockAddr;                              // Address Socket
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);   // File Descriptor
    int buffer_size = 3000;         // Buffer Lenght
    char buffer[buffer_size];       // Create Buffer
    int n;                          // Current Buffer Lenght

    if(-1 == SocketFD){
        perror("Can not create socket");
        exit(1);
    }

    int reuse = 1;
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
 
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(80);
    stSockAddr.sin_addr.s_addr = INADDR_ANY;

    //Avoid bind error if the socket was not close()
    setsockopt(SocketFD,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));
    
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

    std::string text;

    std::cout << "Server Created\n";
    int idx;

    while(true){
    
        int ConnectFD = accept(SocketFD, NULL, NULL);
        if(0 > ConnectFD){
            perror("Error accept failed");
            close(SocketFD);
            exit(0);
        }

        bzero(buffer,buffer_size);                      // put zero to the buffer
        n = read(ConnectFD, buffer, buffer_size);       // read msg in the buffer

        if(n > 0){
            printf("%s\n", buffer);
            set_direction(buffer);
            
            n = write(ConnectFD, text.c_str(), text.size());
        }        
        close(ConnectFD);
    }
    
    close(SocketFD);

    return 0;
}
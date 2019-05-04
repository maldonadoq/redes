#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp> 

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

FILE *file_manager;

std::vector<std::string> routes;
enum pages{
    pmain,
    pone,
    ptwo,
    pform,
    presult,
    p404
};

std::string load_html(std::string _file){
    std::string html = "";

    file_manager = fopen(_file.c_str(), "r");
    if (file_manager == NULL){
        std::cout << _file << "\tload: wrong\n";
    }
    else{
        char* line = NULL;
        size_t len = 0;
        while((getline(&line, &len, file_manager)) != -1) {
            // printf("%s", line);
            html += line;
        }
        fclose(file_manager);
        if(line)
            free(line);

        std::cout << _file << "\tload: ok\n";
    }

    return html;
}

void init_routes(){
    routes.resize(6);

    routes[pmain]   = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n" + load_html("html/page_main.html");
    routes[pone]    = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n" + load_html("html/page_one.html");
    routes[ptwo]    = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n" + load_html("html/page_two.html");
    routes[pform]   = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n" + load_html("html/page_form.html");
    routes[presult] = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n" + load_html("html/page_result.html");
    routes[p404]    = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n" + load_html("html/page_404.html");

    std::cout << "\n";
}

std::pair<int, bool> there_is(std::string _s, char _p){
    for(unsigned i=0; i<_s.size(); i++){
        if(_s[i] == _p){
            return std::make_pair(i, true);
        }
    }

    return std::make_pair(-1, false);
}

std::string get_direction(std::string _recv){
    std::vector<std::string> result;
    boost::split(result, _recv, boost::is_any_of(" ")); 

    std::pair<int, bool> res = there_is(result[1], '?');
    
    // std::cout << result[1] << "\n";

    if(!res.second){
        if(result[1] == "/main")
            return routes[pmain];
        else if(result[1] == "/one")
            return routes[pone];
        else if(result[1] == "/two")
            return routes[ptwo];
        else if(result[1] == "/form")
            return routes[pform];
        else if(result[1] == "/result")
            return routes[presult];
    }
    else{        
        std::string sub = result[1].substr(res.first+1);
        // std::cout << "sub: " << sub << "\n";

        std::vector<std::string> par;
        boost::split(par, sub, boost::is_any_of("&"));

        if(par.size() == 3){
            int a,b;
            int pos;

            pos = par[0].find("=");
            a = stoi(par[0].substr(pos+1));

            pos = par[1].find("=");
            b = stoi(par[1].substr(pos+1));

            pos = par[2].find("=");

            std::string thtml = routes[presult];
            boost::replace_all(thtml, "PA", std::to_string(a));
            boost::replace_all(thtml, "PB", std::to_string(b));

            if(par[2].substr(pos+1) == "add"){
                boost::replace_all(thtml, "PO", "+");
                boost::replace_all(thtml, "PR", std::to_string(a+b));
            }
            else{
                boost::replace_all(thtml, "PO", "-");
                boost::replace_all(thtml, "PR", std::to_string(a-b));
            }

            return thtml;
        }
    }

    return routes[p404];
}

// build: g++ server.cpp -o server.out  
// run:   sudo ./server.out
int main(int argc, char const *argv[]){
    struct sockaddr_in stSockAddr;                              // Address Socket
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);   // File Descriptor
    int buffer_size = 1024;         // Buffer Lenght
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

    init_routes();

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

            text = get_direction(buffer);
            write(ConnectFD, text.c_str(), text.size());
        }
        close(ConnectFD);

        std::cout << "---------------------\n";
    }
    
    close(SocketFD);

    return 0;
}


#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<arpa/inet.h>
#include<fcntl.h> // for open
#include<unistd.h> // for close
#include<pthread.h>
#include<chrono>
#include<thread>
#include<sstream>

// Deivid

using namespace std;

char client_message[2000]; //Buffer de recepcion
char buffer[1024];

int main(){
  int serverSocket, newSocket;
  struct sockaddr_in serverAddr;
  struct sockaddr_storage serverStorage; //sockaddr_storage equivalente a sockaddr_in o sockaddr_in6
  socklen_t addr_size;

  serverSocket = socket(PF_INET, SOCK_STREAM, 0);
  serverAddr.sin_family = AF_INET;   // Address family = Internet
  serverAddr.sin_port = htons(80); //Estableciendo el puerto, no puede ser menor a 1024
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");    //Set IP address to localhost
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);   //Set all bits of the padding field to 0
  bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));   //Bind the address struct to the socket
  if(listen(serverSocket,50)==0){ cout<<"[Listening]"<<endl; }   //Listen on the socket, with 40 max connection requests queued
  else{ cout<<"[Not listening]"<<endl; }
  pthread_t tid[60];
  pthread_t chariot;

  string formater;
  string modular;

  while(1){
    addr_size = sizeof(serverStorage);
    newSocket = accept(serverSocket, (struct sockaddr*)&serverStorage, &addr_size);
    recv(newSocket, client_message, 1024, 0);

    formater = client_message;
    stringstream carriot(formater);
    cout<<"[Recibido]: "<<endl;
    getline(carriot, modular,'/');
    getline(carriot, modular,' ');
    cout<<"[Instruction]: "<<modular<<endl;
    //cout<<"[Instruction]: "<<formater<<endl;


    if(modular == "child_1.html")
      formater = "HTTP/1.1 200 Okay \r\nContent-Type: text/html; charset=ISO-8859-4 \r\n\r\n<h1>Child_1</h1> <a href=\"/ \">main</a> <a href=\"/child_2.html\">child_2</a>";
    else if(modular == "child_2.html")
      formater = "HTTP/1.1 200 Okay \r\nContent-Type: text/html; charset=ISO-8859-4 \r\n\r\n<h1>Child_2</h1> <a href=\"/ \">main</a> <a href=\"/child_1.html\">child_1</a>";
    else
      formater = "HTTP/1.1 200 Okay \r\nContent-Type: text/html; charset=ISO-8859-4 \r\n\r\n<h1>Main</h1><a href=\"/child_1.html\">child_1</a> <a href=\"/child_2.html\">child_2</a>";


    strcpy(buffer, formater.c_str());
    send(newSocket, buffer, sizeof(buffer), 0);
    memset(buffer, '\0', sizeof(buffer));

    //recv(newSocket, message, 1024, 0)
    //formater

  }
  close(newSocket);
  return 0;

}

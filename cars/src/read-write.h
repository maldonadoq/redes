#ifndef _READ_WRITE_H_
#define _READ_WRITE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <chrono>
#include "utils.h"

void print_road(std::string _road, unsigned r, unsigned c){
	unsigned i,j;

	std::string tmp = "";
	for(j=0; j<(c-3)/2; j++)
		tmp += " ";

	for(i=0; i<r; i++){
		std::cout << "|" << tmp << _road[i] << tmp << "|\n";
	}
}

void thread_write(int _sockFD){
	std::string text;

	while(true){
		getline(std::cin, text);
		if(text == "end"){
			break;
		}
		send(_sockFD, text.c_str(), text.size(), 0);
	}
}

void thread_read(int _sockFD, unsigned _rsize, unsigned _csize){
	unsigned buffer_size = _rsize*_csize;
	char buffer[buffer_size];

	int n;

	while(true){
		memset(&buffer, 0, buffer_size);
		n = recv(_sockFD, buffer, buffer_size, 0);
		if(n == 0){
			system("clear");
			std::cout << "you are lose --- write end to finished\n";
			break;
		}
		else if(n < 0){
			perror("error receiving text");
		}
		else{
			system("clear");
			print_table_from_str(std::string(buffer),_rsize,_csize);
		}
	}


}

#endif
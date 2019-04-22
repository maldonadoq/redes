#ifndef _READ_WRITE_H_
#define _READ_WRITE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <chrono>

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
		send(_sockFD, text.c_str(), text.size(), 0);
	}
}

void thread_read(int _sockFD, unsigned row, unsigned col){
	unsigned buffer_size = row;
	char buffer[buffer_size];

	while(true){
		memset(&buffer, 0, buffer_size);
		if(recv(_sockFD, buffer, buffer_size, 0) > 0){
			// std::cout << buffer << "\n";
			system("clear");
			print_road(buffer, row, col);
		}
	}
}

#endif
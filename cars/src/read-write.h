#ifndef _READ_WRITE_H_
#define _READ_WRITE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <chrono>
#include "utils.h"

void thread_write(int _sockFD){
	std::string text;
	char tmp = 27;
	while(true){
		// getline(std::cin, text);
		text = getch();
		if(text[0] == tmp){
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
		system("clear");
		if(n == 0){			
			// std::cout << "You are lose\t[Write \"end\" to finished]\n";
			break;
		}		
		else if(n < 0){
			perror("error receiving text");
		}
		else{
			if(n == 1){
				if(buffer[0] = '0')
					std::cout << "This avatar is already used!\n";
				else
					std::cout << "Avatar accepted!\n";
			}
			else
				print_table_from_str(std::string(buffer),_rsize,_csize);
		}		
	}
}

#endif
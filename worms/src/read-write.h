#ifndef _READ_WRITE_H_
#define _READ_WRITE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <chrono>
#include <thread>

#include "utils.h"

static char DIR = 'd';

void thread_write(int _sockFD){
	std::string text;

	while(true){
		// getline(std::cin, text);
		// send(_sockFD, text.c_str(), text.size(), 0);
		DIR = getch();
	}
}

void thread_write_speed(int _sockFD, int _speed){
	std::string text;

	while(true){
		// sleep(_speed);
		std::this_thread::sleep_for(std::chrono::milliseconds(_speed));
		
		text = std::string(1, DIR);
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
			break;
		}
		else if(n > 0){
			system("clear");			
			print_table_from_str(std::string(buffer),_rsize,_csize);
		}
	}
}

#endif
#ifndef _READ_WRITE_H
#define _READ_WRITE_H

#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <string>

void thread_write(int _sockFD){
	std::string text;
	std::string tmp;

	while(true){
		std::cout << "> ";		
		getline(std::cin, text);
		tmp = std::to_string(text.size());
		write(_sockFD, tmp.c_str(), 1);
		write(_sockFD, text.c_str(), text.size());
	}
}

void thread_read(int _sockFD){
	char buffer1[1];
	char *buffer2;
    
	while(true){
		if(read(_sockFD, buffer1, 1) >= 1){
			buffer2 = new char[atoi(buffer1)];
			if(read(_sockFD, buffer2, atoi(buffer1)) >= 1)
				printf("< %s\n",buffer2);
		}		
	}
}

void thread_read_write(int _sockFD){
	std::string text;
	unsigned buffer_size = 256;
	char buffer[buffer_size];

	while(true){
		std::this_thread::sleep_for(std::chrono::milliseconds(10000));
		if(read(_sockFD, buffer, buffer_size) > 0){			
			printf("< %s\n",buffer);

			std::cout << "> ";
			getline(std::cin, text);
			if(write(_sockFD, text.c_str(), text.size()) < 0){
				perror("Error writing data");
			}
		}		
	}
}

void thread_write_read(int _sockFD){
	std::string text;
	unsigned buffer_size = 256;
	char buffer[buffer_size];

	while(true){
		std::cout << "> ";
		getline(std::cin, text);
		if(write(_sockFD, text.c_str(), text.size()) > 0){
			std::this_thread::sleep_for(std::chrono::milliseconds(10000));
			if(read(_sockFD, buffer, buffer_size) > 0)
				printf("< %s\n",buffer);
		}		
	}	
}

#endif
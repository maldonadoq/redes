#ifndef _READ_WRITE_H
#define _READ_WRITE_H

#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#define N 2 			//	[0-99]
#define BS_MAX	102		//	buffer size max + N 

std::string to_str(int n, unsigned s){
	std::string str = std::to_string(n);

	for(unsigned i=str.size(); i<s; i++){
		str = '0' + str;
	}

	return str;
}

std::string read_buffer(std::string _str, int _init, int _end){
	std::string str = "";
	for(int i=_init; i<_end; i++){
		str += _str[i];
	}

	return str;
}

void thread_write(int _sockFD){
	std::string text;
	std::string tmp;

	while(true){
		getline(std::cin, text);
		text = to_str(text.size(), N) + text;
		write(_sockFD, text.c_str(), text.size());
	}
}

void thread_read(int _sockFD){	
	char buffer[BS_MAX];
	int  bsread;

	while(true){
		if(read(_sockFD, buffer, BS_MAX) > 0){
			bsread = stoi(read_buffer(buffer, 0, N));
			std::cout << read_buffer(buffer, N, bsread+N) << "\n";
		}		
	}
}

#endif
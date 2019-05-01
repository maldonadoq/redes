#ifndef _READ_WRITE_H_
#define _READ_WRITE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <chrono>
#include <math.h>

std::string read_buffer(std::string _s, unsigned t){
	std::string tmp = "";
	for(unsigned i=0; i<t; i++)
		tmp += _s[i];

	return tmp;
}

std::string str_complete(std::string _s, unsigned t){
	std::string tmp = _s;
	for(unsigned i=_s.size(); i<t; i++)
		tmp = '0'+tmp;

	return tmp;
}

void thread_write(int _sockFD, int _loop, int _size){
	std::string text;
	int i = 0;
	srand(time(NULL));

	int tm = pow(10, _size);
	while(i < _loop){
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		text = str_complete(std::to_string((rand()%tm) - 100), _size);
		
		if( send(_sockFD, text.c_str(), text.size(), 0) > 0){
			i++;
		}
	}

	text = "";
	send(_sockFD, text.c_str(), text.size(), 0);
}

void thread_read(int _sockFD){
	unsigned buffer_size = 256;
	char buffer[buffer_size];

	while(true){
		memset(&buffer, 0, buffer_size);
		if(recv(_sockFD, buffer, buffer_size, 0) > 0){			
			std::cout << buffer << "\n";
		}
	}
}

#endif
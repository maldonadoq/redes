#ifndef _UTILS_H_
#define _UTILS_H_

#include <vector>
#include <termios.h>
#include <stdio.h>

static struct termios told, tnew;

void print_vector(std::vector<std::string> _vec){
	for(unsigned i=0; i<_vec.size(); i++){
		std::cout << _vec[i] << "\n";
	}
}

std::string add_zeros_left(int _number, unsigned _size){
	std::string tmp = std::to_string(_number);
	for(unsigned i=tmp.size(); i<_size; i++){
		tmp = '0'+tmp;
	}

	return tmp;
}

void initTermios() {
	tcgetattr(0, &told);
	tnew = told;
	tnew.c_lflag &= ~ICANON;
	tnew.c_lflag &= ~ECHO;
	tcsetattr(0, TCSANOW, &tnew);
}

void resetTermios() {
	tcsetattr(0, TCSANOW, &told);
}

char getch(){
	char ch;
	initTermios();

	ch = getchar();

	resetTermios();
	return ch;
}

#endif
#ifndef _UTILS_H_
#define _UTILS_H_

#include <termios.h>
#include <stdio.h>

static struct termios told, tnew;

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

void print_table(char **_table, unsigned _r, unsigned _c){
	for(unsigned i=0; i<_r; i++){
		for(unsigned j=0; j<_c; j++)
			std::cout << _table[i][j];
		std::cout << "\n";
	}
}

void print_table_from_str(std::string _t, unsigned _r, unsigned _c){
	// std::cout << _t << "\n";
	for(unsigned i=0; i<_r; i++){
		for(unsigned j=0; j<_c; j++)
			std::cout << _t[(i*_c)+j];
		std::cout << "\n";
	}
}

std::string table_to_str(char **_t, unsigned _r, unsigned _c){
	std::string tmp = "";

	for(unsigned i=0; i<_r; i++){
		for(unsigned j=0; j<_c; j++){
			tmp += _t[i][j];
		}						
	}

	return tmp;
}

#endif
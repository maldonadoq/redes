#ifndef _UTILS_H_
#define _UTILS_H_

#include <iostream>

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
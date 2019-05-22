#ifndef _UTILS_H_
#define _UTILS_H_

#include <vector>

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



#endif
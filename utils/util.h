#ifndef _UTIL_H_
#define _UTIL_H_

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

#endif
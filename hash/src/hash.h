#ifndef _HASH_H_
#define _HASH_H_

#include <iostream>
#include <vector>
#include <fstream>
#include <math.h>
#include "../../utils/util.h"

#define N 2 					//	[0-99]
#define BufferSize 50			//	block of size 20
#define Base (int)pow(10,N);	//	Base


static std::string read_file(char const* filename){
	std::ifstream ifs(filename, std::ios::binary|std::ios::ate);
	std::ifstream::pos_type pos = ifs.tellg();

	std::vector<char>  vec_buffer(pos);

	ifs.seekg(0, std::ios::beg);
	ifs.read(&vec_buffer[0], pos);

	std::string ret = std::string(vec_buffer.begin(), vec_buffer.end());
	
	return ret;
}

std::vector<std::string> partition(std::string _msg, int _size){
    std::vector<std::string> vect;
    std::string temp;

    int nb = ceil(_msg.size()/_size);

    for(int i=0; i<nb; i++){
        temp = "";
        for(int j=0; j<_size; j++){
            if((i*_size)+j < _msg.size())
                temp += _msg[(i*_size)+j];
        }
        vect.push_back(temp);
    }

    return vect;
}

std::string my_hash(std::string _msg){
	int hash = 0;
	for(unsigned i=0; i<_msg.size(); i++){
		hash += _msg[i];
	}

	hash = hash%Base;
	std::string shash = std::to_string(hash);
	for(unsigned i=shash.size(); i<N; i++)
		shash = '0'+shash;

	return shash;
}

#endif
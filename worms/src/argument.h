#ifndef _ARGUMENT_H_
#define _ARGUMENT_H_

#include "data.h"
#include "worm.h"

class TArg{
public:
	TData *m_client;
	TWorm *m_worm;
	TArg(){
		this->m_client = new TData();
		this->m_worm = new TWorm();
	}

	~TArg(){
		delete this->m_client;
		delete this->m_worm;	
	}
};

#endif
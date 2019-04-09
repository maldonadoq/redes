#ifndef _THREAD_H_
#define _THREAD_H_

#include <pthread.h>
#include <string>
#include "data.h"

class TThread{
private:
	static pthread_mutex_t m_mutex;
public:
	pthread_t m_tid;

	TThread();
	~TThread();

	bool Create(void *, void *);
	bool Join();

	static bool InitMutex();
	static bool LockMutex(std::string);
	static bool UnlockMutex(std::string);
};

pthread_mutex_t TThread::m_mutex;

TThread::TThread(){

}

bool TThread::Create(void *callback, void * arg){

	pthread_create(&this->m_tid, NULL, (void *(*)(void *))callback, arg);
	return true;
}

bool TThread::Join(){
	pthread_join(this->m_tid, NULL);
	return true;
}

bool TThread::InitMutex(){
	if(pthread_mutex_init(&TThread::m_mutex, NULL) < 0)
		return false;

	return true;
}

bool TThread::LockMutex(std::string identifier){
	if(pthread_mutex_lock(&TThread::m_mutex) == 0)
		return true;

	return false;
}

bool TThread::UnlockMutex(std::string identifier){
	if(pthread_mutex_unlock(&TThread::m_mutex) == 0)
		return true;

	return false;
}

#endif
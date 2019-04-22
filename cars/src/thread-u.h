#ifndef _THREAD_U_H_
#define _THREAD_U_H_

#include <pthread.h>
#include <string>
#include <thread>
#include <mutex>
#include "car.h"
#include "socket-info.h"

class TThread{
public:
	std::thread m_tid;

	TThread();
	~TThread();

	void Create(void (*callback)(TSocket *, TCar *), TSocket *, TCar *);
	void Join();
};

TThread::TThread(){

}

void TThread::Create(void (*callback)(TSocket *, TCar *), TSocket *dat, TCar *car){
	this->m_tid = std::thread(callback, dat, car);
}

void TThread::Join(){
	m_tid.join();
}

#endif
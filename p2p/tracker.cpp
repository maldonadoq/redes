#include <iostream>
#include "inc/tracker.h"

// build: g++ tracker.cpp -o tracker.out -pthread
int main(int argc, char const *argv[]){
	int port = 7777;
	int bs = 16;

	TTracker *t = new TTracker(bs, port);
	t->Execute();
	return 0;
}
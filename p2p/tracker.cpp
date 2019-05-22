#include <iostream>
#include "inc/tracker.h"

// build: g++ tracker.cpp -o tracker.out -pthread
int main(int argc, char const *argv[]){
	int port = 7777;
	int buffer_size = 16;

	TTracker *t = new TTracker(buffer_size);
	
	t->Create(port);
	t->Listening();
	return 0;
}
#include <iostream>
#include "inc/protocol.h"

int main(int argc, char const *argv[]){
	int size = 20;
	TProtocol testing(size);

	testing.Sending("Lorem ipsum dolor sit amet, consectetur adipisicing elit.", 20);
	return 0;
}
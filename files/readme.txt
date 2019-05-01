g++ -std=c++11 -pthread server.cpp -o server.out
	./server.out

g++ -std=c++11 -pthread client.cpp -o client.out
	./client.out 10001
	./client.out 10002
	./client.out 10003	

				(necesariamente en ese orden)
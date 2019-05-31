all: tracker peer

tracker: tracker.cpp
	g++ -std=c++17 -o tracker.out tracker.cpp -pthread

peer: peer.cpp
	g++ -std=c++17 -o peer.out peer.cpp -pthread

clean:
	rm -f *.out
CC		= g++ -std=c++17
LIBS	= -pthread -lsqlite3

all: front slave

front: front.cpp
	$(CC) front.cpp -o front.out $(LIBS)

slave: slave.cpp
	$(CC) slave.cpp -o slave.out $(LIBS)

data:
	rm db/*.db

# delete lib with clean 
clean:
	rm -f *.out
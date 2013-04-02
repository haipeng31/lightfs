OBJ = 
CFLAGS = -std=c++11 -c -g
CC = g++
INode.o:INode.cpp
	$(CC) $(CFLAGS) INode.cpp

HashTable.o:HashTable.cpp
	$(CC) $(CFLAGS) HashTable.cpp

clean:
	rm *.o

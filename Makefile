OBJ = INode.o HashTable.o DirTree.o netlib/base/Logging.o  
CFLAGS = -std=c++11 -c -g
CC = g++

testdirtree:$(OBJ) testdirtree.o
	$(CC) -o testdirtree $(OBJ) testdirtree.o

testdirtree.o:testdirtree.cpp
	$(CC) $(CFLAGS) testdirtree.cpp

INode.o:INode.cpp
	$(CC) $(CFLAGS) INode.cpp

HashTable.o:HashTable.cpp
	$(CC) $(CFLAGS) HashTable.cpp

DirTree.o:DirTree.cpp
	$(CC) $(CFLAGS) DirTree.cpp

clean:
	rm *.o

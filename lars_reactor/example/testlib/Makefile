CXX=g++
CFLAGS=-g -O2 -Wall -fPIC -Wno-deprecated 

INC=-I../../include
LIB=-L../../lib -llreactor 
OBJS = $(addsuffix .o, $(basename $(wildcard *.cc)))

all:
	$(CXX) -o hello_lars $(CFLAGS)  hello_lars.cpp $(INC) $(LIB)

clean:
	-rm -f *.o hello_lars

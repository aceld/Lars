CXX = g++
CFLAGS = -g -O2 -Wall -Wno-deprecated

LARS_REACTOR=../../../lars_reactor
LARS_REACTOR_H =$(LARS_REACTOR)/include
LARS_REACTOR_LIB=$(LARS_REACTOR)/lib  -llreactor

all:
		$(CXX) $(CFLAGS) -o example example.cpp -I$(LARS_REACTOR_H) -I../lars_api ../lib/liblarsclient.a -lprotobuf


.PHONY: clean

clean:
		-rm ./example

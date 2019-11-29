CXX = g++
CFLAGS = -g -O2 -Wall -Wno-deprecated

LARS_REACTOR=../../../lars_reactor
LARS_REACTOR_H =$(LARS_REACTOR)/include
LARS_REACTOR_LIB=$(LARS_REACTOR)/lib  -llreactor

all:
		$(CXX) $(CFLAGS) -o example example.cpp -I$(LARS_REACTOR_H) -I../lars_api ../lib/liblarsclient.a -lprotobuf
		$(CXX) $(CFLAGS) -o simulator simulator.cpp -I$(LARS_REACTOR_H) -I../lars_api ../lib/liblarsclient.a -lprotobuf
		$(CXX) $(CFLAGS) -o qps qps.cpp -I$(LARS_REACTOR_H) -I../lars_api ../lib/liblarsclient.a -lprotobuf -lpthread
		$(CXX) $(CFLAGS) -o get_host get_host.cpp -I$(LARS_REACTOR_H) -I../lars_api ../lib/liblarsclient.a -lprotobuf
		$(CXX) $(CFLAGS) -o get_route get_route.cpp -I$(LARS_REACTOR_H) -I../lars_api ../lib/liblarsclient.a -lprotobuf
		$(CXX) $(CFLAGS) -o report report.cpp -I$(LARS_REACTOR_H) -I../lars_api ../lib/liblarsclient.a -lprotobuf


.PHONY: clean

clean:
		-rm ./example ./simulator ./qps ./get_host ./get_route ./report

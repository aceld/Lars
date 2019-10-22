TARGET= bin/lars_lb_agent
CXX=g++
CFLAGS=-g -O2 -Wall -Wno-deprecated

BASE=../base
BASE_H=$(BASE)/include

PROTO = $(BASE)/proto
PROTO_H = $(BASE)/proto

LARS_REACTOR=../lars_reactor
LARS_REACTOR_H =$(LARS_REACTOR)/include
LARS_REACTOR_LIB=$(LARS_REACTOR)/lib  -llreactor

MYSQL=$(BASE)/mysql-connector-c
MYSQL_H=$(MYSQL)/include
MYSQL_LIB=$(MYSQL)/lib/libmysqlclient.a

OTHER_LIB = -lpthread -ldl -lprotobuf
SRC= ./src
INC= -I./include -I$(BASE_H) -I$(LARS_REACTOR_H) -I$(MYSQL_H) -I$(PROTO_H)

LIB= $(MYSQL_LIB) -L$(LARS_REACTOR_LIB) $(OTHER_LIB) 


OBJS = $(addsuffix .o, $(basename $(wildcard $(SRC)/*.cpp)))
OBJS += $(PROTO)/lars.pb.o

$(TARGET): $(OBJS)
	mkdir -p bin
	$(CXX) $(CFLAGS) -o $(TARGET) $(OBJS) $(INC) $(LIB)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c -o $@ $< $(INC) 

.PHONY: clean

clean:
	-rm -f src/*.o  $(PROTO)/lars.pb.o $(TARGET)

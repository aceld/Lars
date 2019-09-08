TARGET=lib/liblreactor.a
CXX=g++
CFLAGS=-g -O2 -Wall -fPIC -Wno-deprecated

SRC=./src

INC=-I./include
OBJS = $(addsuffix .o, $(basename $(wildcard $(SRC)/*.cpp)))

$(TARGET): $(OBJS)
	mkdir -p lib
	ar cqs $@ $^

%.o: %.cpp
	$(CXX) $(CFLAGS) -c -o $@ $< $(INC)

.PHONY: clean

clean:
	-rm -f src/*.o $(TARGET)

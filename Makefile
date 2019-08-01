PROJECT := $(shell pwd)
MAIN 	:= $(PROJECT)/src/Main.cpp
SRC  	:= $(wildcard $(PROJECT)/src/*.cpp $(PROJECT)/src/base/*.cpp)
override SRC := $(filter-out $(MAIN), $(SRC))
OBJECT  := $(patsubst %.cpp, %.o, $(SRC))
BIN 	:= $(PROJECT)/bin
TARGET  := webd
CXX     := g++
LIBS    := -lpthread
INCLUDE	:= -I ./usr/local/lib
CFLAGS  := -std=c++11 -g -Wall -O3 -D_PTHREADS
CXXFLAGS:= $(CFLAGS)

all : $(BIN)/$(TARGET)

$(BIN)/$(TARGET) : $(OBJECT) $(PROJECT)/src/Main.o
	[ -e $(BIN) ] || mkdir $(BIN)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

clean :
	find . -name '*.o' | xargs rm -f
	find . -name $(TARGET) | xargs rm -f

INCLUDE_DIRS ?= -I../../../build/release-install-cpp11/include/
LIB_DIRS ?= -L../../../build/release-install-cpp11/lib/ 
LIBS ?= -lmuduo_net -lmuduo_base -lpthread 

all: client server # 默认情况下, 会视为第一个为默认目标, 因此需要使用all来将两个都作为默认目标

client: client.cc
	g++ -o $@ $^ -g -std=c++11 $(INCLUDE_DIRS) $(LIB_DIRS) $(LIBS)

server: server.cc
	g++ -o $@ $^ -g -std=c++11 $(INCLUDE_DIRS) $(LIB_DIRS) $(LIBS)

.PHONY:clean
clean:
	rm client server

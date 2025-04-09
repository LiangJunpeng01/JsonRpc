all: async package promise

async: asynctest.cpp
	g++ -o $@ $^ -g -std=c++11

package: packagedtest.cpp
	g++ -o $@ $^ -g -std=c++11

promise: promisetest.cpp
	g++ -o $@ $^ -g -std=c++11


.PHONY:clean
clean:
	rm async package promise

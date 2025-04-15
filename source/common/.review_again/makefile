all: uuid log

uuid: uuid.cc
	g++ -o $@ $^ -g -std=c++11

log: log.cc
	g++ -o $@ $^ -g -std=c++11


.PHONY: clean

clean:
	rm uuid log

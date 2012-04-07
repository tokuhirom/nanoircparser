all: nanoircparser.hpp
	g++ -Wall -g -DTEST_NANOIRCPARSER t/test.cc -o a.out
	./a.out

nanoircparser.hpp: nanoircparser.src.re
	re2c -i nanoircparser.src.re > nanoircparser.hpp

.PHONY: all

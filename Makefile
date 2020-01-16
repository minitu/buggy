DEFS=-DDEBUG

all: test

test: test.cu buggy.h
	nvcc -std=c++11 -o $@ $< -I ./ $(DEFS)

clean:
	rm -f *.o test

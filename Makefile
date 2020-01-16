all: test

test: test.cpp guddy.h
	g++ -o $@ $< -I ./

clean:
	rm -f *.o test

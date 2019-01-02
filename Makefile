CC=g++-8
INCLUDES=-I${PWD}/include
FLAGS=-std=c++17 -ffast-math

run: main
	build/main

clean:
	rm -f build/*

test: score-test

## Wildcard build objects
# Exception: main-test is in test/, not in src/.
build/%-test.o: test/%-test.cpp
	$(CC) $(FLAGS) -c $^ -o $@

%-test: build/main-test.o test/%-test.cpp
	$(CC) $(FLAGS) $^ -o build/$@
	build/$@

main: main.cpp
	$(CC) $(INCLUDES) $(FLAGS) -o build/$@ $^ 

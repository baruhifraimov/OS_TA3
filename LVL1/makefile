CXX = g++
FLAGS = -Wall -g -std=c++20

all: main

main: main.o
	$(CXX) $(FLAGS) $< -o $@
	
%.o: %.cpp
	$(CXX) $(FLAGS) -c $< -o $@

clean:
	rm -f *.o main

.PHONY: all clean
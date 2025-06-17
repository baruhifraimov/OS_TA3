CXX = g++
FLAGS = -Wall -g -std=c++20 -pg

all:  graham

graham: graham.o
	$(CXX) $(FLAGS) $< -o $@

%.o: %.cpp
	$(CXX) $(FLAGS) -c $< -o $@

clean:
	rm -f *.o  graham *.out

.PHONY: all clean
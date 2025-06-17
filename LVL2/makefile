CXX = g++
FLAGS = -Wall -g -std=c++20 -pg

all: andrew graham

andrew: andrew.o
	$(CXX) $(FLAGS) $< -o $@

graham: graham.o
	$(CXX) $(FLAGS) $< -o $@

%.o: %.cpp
	$(CXX) $(FLAGS) -c $< -o $@

clean:
	rm -f *.o andrew graham *.out

.PHONY: all clean
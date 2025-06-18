CXX = g++
FLAGS = -Wall -g -std=c++20 -pg
TARGET = server client

all: $(TARGET)

client: client.o
	$(CXX) $(FLAGS) $< -o $@

server: server.o
	$(CXX) $(FLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(FLAGS) -c $< -o $@

clean:
	rm -f *.o  $(TARGET) *.out

.PHONY: all clean 
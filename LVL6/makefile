CXX = g++
FLAGS = -Wall -g -std=c++20 -fPIC
LIB = libreactor.so
TARGET = server client

all: $(LIB) $(TARGET)

client: client.o
	$(CXX) $(FLAGS) $< -o $@

server: server.o $(LIB)
	$(CXX) $(FLAGS) $< -L. -lreactor -Wl,-rpath=. -o $@

server.o: server.cpp
	$(CXX) $(FLAGS) -c $< -o $@

client.o: client.cpp
	$(CXX) $(FLAGS) -c $< -o $@

$(LIB): reactor.o
	$(CXX) -shared $< -o $@

reactor.o: reactor.cpp reactor.hpp
	$(CXX) $(FLAGS) -c $< -o $@

clean:
	rm -f *.o  $(LIB) *.out $(TARGET)

.PHONY: all clean
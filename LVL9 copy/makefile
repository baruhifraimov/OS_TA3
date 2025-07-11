CXX = g++
FLAGS = -Wall -g -std=c++20 -pg
LIB_FLAGS = -Wall -g -std=c++20 -fPIC -pthread
TARGET = server client
LIB = libreactorproactor.so

all: $(LIB) $(TARGET)

# Create shared library
$(LIB): reactor.o proactor.o
	$(CXX) -shared $^ -o $@ -pthread

# Client compilation
client: client.o
	$(CXX) $(FLAGS) $< -o $@

# Server compilation using the library
server: server.o $(LIB)
	$(CXX) $(FLAGS) server.o -L. -lreactorproactor -Wl,-rpath=. -o $@ -pthread

# Special compilation for library objects
reactor.o: reactor.cpp reactor.hpp
	$(CXX) $(LIB_FLAGS) -c $< -o $@

proactor.o: proactor.cpp proactor.hpp
	$(CXX) $(LIB_FLAGS) -c $< -o $@

# General rule for application objects
%.o: %.cpp
	$(CXX) $(FLAGS) -pthread -c $< -o $@

clean:
	rm -f *.o $(TARGET) $(LIB) *.out

.PHONY: all clean
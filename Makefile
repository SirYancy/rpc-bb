CXX = g++
FLAGS += -std=c++11 -pthread -Wall -g

all: server_tcp client_tcp coordinator_tcp

# Objects

Article.o: Article.h Article.cpp
	$(CXX) $(FLAGS) -c Article.cpp

backend.o: backend.h backend.cpp
	$(CXX) $(FLAGS) -c backend.cpp

tcp.o: tcp.h tcp.cpp
	$(CXX) $(FLAGS) -c tcp.cpp

client_tcp.o: client_tcp.cpp
	$(CXX) $(FLAGS) -c client_tcp.cpp
server_tcp.o: server_tcp.cpp
	$(CXX) $(FLAGS) -c server_tcp.cpp

coordinator_tcp.o: coordinator_tcp.cpp
	$(CXX) $(FLAGS) -c coordinator_tcp.cpp

# Executables

server_tcp: Article.o tcp.o backend.o server_tcp.o 
	$(CXX) $(FLAGS) -o server_tcp backend.o tcp.o Article.o server_tcp.o

client_tcp: Article.o client_tcp.o tcp.o backend.o
	$(CXX) $(FLAGS) -o client_tcp Article.o backend.o client_tcp.o tcp.o

coordinator_tcp: Article.o coordinator_tcp.o tcp.o backend.o
	$(CXX) $(FLAGS) -o coordinator_tcp Article.o backend.o coordinator_tcp.o tcp.o

clean:
	$(RM) core *.o server_tcp client_tcp coordinator_tcp

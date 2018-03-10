CXX = g++
FLAGS += -std=c++11 -pthread -Wall -g

all: bb_server tcp_client

# Objects

Article.o: Article.h Article.cpp
	$(CXX) $(FLAGS) -c Article.cpp

backend.o: backend.h backend.cpp
	$(CXX) $(FLAGS) -c backend.cpp

tcp.o: tcp.h tcp.c
	$(CXX) $(FLAGS) -c tcp.c

bb_server.o: bb_server.cpp
	$(CXX) $(FLAGS) -c bb_server.cpp

# Executables

bb_server: Article.o tcp.o backend.o bb_server.o 
	$(CXX) $(FLAGS) -o server_tcp backend.o tcp.o Article.o bb_server.o

tcp_client: Article.o client_tcp.o tcp.o backend.o
	$(CXX) $(FLAGS) -o client_tcp Article.o backend.o client_tcp.o tcp.o

clean:
	$(RM) core *.o server_tcp client_tcp

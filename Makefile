CXX = g++
FLAGS += -std=c++11 -pthread -g -O

all: bb_server tcp_server tcp_client

# Objects

Article.o: Article.h Article.cpp
	$(CXX) $(FLAGS) -c Article.cpp

tcp.o: tcp.h tcp.c
	$(CXX) $(FLAGS) -c tcp.c

bb_server.o: bb_server.cpp
	$(CXX) $(FLAGS) -c bb_server.cpp

# Executables

bb_server: bb_server.o Article.o tcp.o 
	$(CXX) $(FLAGS) -c bb_server bb_server.o tcp.o Article.o


tcp_server: server_tcp.o tcp.o
	$(CC) -o server_tcp server_tcp.o tcp.o -lnsl -lpthread

tcp_client: client_tcp.o tcp.o
	$(CC) -o client_tcp client_tcp.o tcp.o -lnsl -lpthread

clean:
	$(RM) core *.o bb_server bb_client

CXX = g++
FLAGS += -std=c++11 -pthread -Wall -g -o

all: bb_server

# Objects

Article.o: Article.h Article.cpp
	$(CXX) $(FLAGS) -c Article.cpp

tcp.o: tcp.h tcp.c
	$(CXX) $(FLAGS) -c tcp.c

bb_server.o: bb_server.cpp
	$(CXX) $(FLAGS) -c bb_server.cpp

# Executables

bb_server: Article.o tcp.o bb_server.o 
	$(CXX) $(FLAGS) -c tcp.o Article.o bb_server.o


clean:
	$(RM) core *.o bb_server bb_client

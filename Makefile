CXX = g++
FLAGS += -std=c++11 -pthread -g -O

all: bb_server

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


clean:
	$(RM) core *.o bb_server bb_client

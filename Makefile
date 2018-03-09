CC = g++
CFLAGS += -std=c++11 -g -O
RPCGEN_FLAGS = -N

all: bb_client bb_server

# Objects for Server

Article.o: Article.h Article.cpp
	$(CC) $(CFLAGS) -c Article.cpp

# Executables
bb_client: bb_clnt.o bb_xdr.o bb_client.o
	$(CC) -o bb_client bb_client.o bb_clnt.o bb_xdr.o -lnsl

bb_server: bb_svc.o backend.o Article.o bb_xdr.o bb_server.o
	$(CC) -o bb_server Article.o backend.o bb_server.o bb_svc.o bb_xdr.o -lnsl

clean:
	$(RM) core *.o bb_server bb_client

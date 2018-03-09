CC = g++
CFLAGS += -std=c++11 -g -O
RPCGEN_FLAGS = -N

all: bb_client bb_server tcp_server tcp_client

# Objects for Server

Article.o: Article.h Article.cpp
	$(CC) $(CFLAGS) -c Article.cpp

backend.o: backend.h backend.cpp
	$(CC) $(CFLAGS) -c backend.cpp

tcp.o: tcp.h tcp.cpp
	$(CC) $(CFLAGS) -c tcp.cpp

# Objects for exec

server_tcp.o: server_tcp.cpp
	$(CC) $(CFLAGS) -c server_tcp.cpp

client_tcp.o: client_tcp.cpp
	$(CC) $(CFLAGS) -c client_tcp.cpp

bb_server.o: bb_server.c bb.h
	$(CC) $(CFLAGS) -c bb_server.c

bb_client.o: bb_client.c bb.h
	$(CC) $(CFLAGS) -c bb_client.c

# RPC gen files

bb_svc.o: bb_svc.c bb.h
	$(CC) $(CFLAGS) -c bb_svc.c

bb_clnt.o: bb_clnt.c bb.h
	$(CC) $(CFLAGS) -c bb_clnt.c

bb_xdr.o: bb_xdr.c bb.h
	$(CC) $(CFLAGS) -c bb_xdr.c

bb_clnt.c bb_svc.c bb_xdr.c bb.h: bb.x
	rpcgen $(RPCGEN_FLAGS) bb.x

	
# Executables
bb_client: bb_clnt.o bb_xdr.o bb_client.o
	$(CC) -o bb_client bb_client.o bb_clnt.o bb_xdr.o -lnsl

bb_server: bb_svc.o backend.o Article.o bb_xdr.o bb_server.o
	$(CC) -o bb_server Article.o backend.o bb_server.o bb_svc.o bb_xdr.o -lnsl

tcp_server: server_tcp.o tcp.o
	$(CC) -o server_tcp server_tcp.o tcp.o -lnsl -lpthread

tcp_client: client_tcp.o tcp.o
	$(CC) -o client_tcp client_tcp.o tcp.o -lnsl -lpthread

clean:
	$(RM) core *.o bb_server bb_client

CC = g++
CFLAGS += -std=c++11 -g -O
RPCGEN_FLAGS =

all: bb_client bb_server

# Objects for Server

Article.o: Article.h Article.cpp
	$(CC) $(CFLAGS) -c Article.cpp

backend.o: backend.h backend.cpp
	$(CC) $(CFLAGS) -c backend.cpp

# Objects for exec

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

clean:
	$(RM) core *.o bb_server bb_client

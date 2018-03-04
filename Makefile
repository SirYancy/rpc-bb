CC = gcc
CFLAGS += -g -O
RPCGEN_FLAGS =

all: bb_client bb_server

# Executables
bb_client: bb_client.o bb_clnt.o bb_xdr.o
	$(CC) -o bb_client bb_client.o bb_clnt.o add_xdr.o -lnsl

bb_server: bb_server.o bb_svc.o backend.o bb_xdr.o
	$(CC) -o bb_server backend.o bb_server.o bb_svc.o add_xdr.o -lnsl


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

add_clnt.c add_svc.c add_xdr.c add.h: add.x
	rpcgen $(RPCGEN_FLAGS) add.x

clean:
	$(RM) core *.o bb_server bb_client

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_CONNECTION 10
#define MAX_LEN 1024

extern int gCoordinatorSocket;
extern int gServerSocket;
extern int gReceivingSocket;

void *client_handler(void *);
void *server_handler(void *);
void *receiving_handler(void *);

bool InitCoordinator(int port);
bool InitServer(int port);
bool InitServerWithHandler(int port, void *(*handler)(void *));

bool ConnectToServer(char *serverIP, int serverPort);
bool ConnectToCoordinator(char *serverIP, int serverPort);

int RequestIndex(void);
void SendACK(void);

bool SendThroughSocket(int socket, char *buffer, int len);
int RecvFromSocket(int socket, char *buffer);

int GetCoordinatorSocket(void);
int GetServerSocket(void);
int GetReceivingSocket(void);


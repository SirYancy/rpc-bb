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

void *client_handler(void *);
void *server_handler(void *);

bool InitCoordinator(int port);
bool InitServer(int port);
<<<<<<< HEAD
bool InitClient(char *serverIP, int serverPort);

bool SendThroughSocket(int socket, char *buffer, int len);
int RecvFromSocket(int socket, char *buffer);

/**
 * tcp.h
 * Project 2 - Distributed bulletin Board
 * Aparna Mahadevan
 * Ming-Hong Yang
 * Eric Kuha
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <map>

#define MAX_CONNECTION 10
#define MAX_LEN 1024

extern int gCoordinatorSocket;
extern int gServerSocket;
extern int gReceivingSocket;

void *client_handler(void *);
void *server_handler(void *);
void *receiving_handler(void *);

bool InitCoordinator(int port, char* type);
bool InitServer(int port, char* type);
bool InitServerWithHandler(int port, void *(*handler)(void *), char *type);

bool ConnectToServer(char *serverIP, int serverPort);
bool ConnectToCoordinator(char *serverIP, int serverPort, int localPort);

int RequestIndex(void);
void SendACK(int socket);

bool SendThroughSocket(int socket, char *buffer, int len);
int RecvFromSocket(int socket, char *buffer);

int GetCoordinatorSocket(void);
int GetServerSocket(void);
int GetReceivingSocket(void);

std::map<int, int> GetMap(void);

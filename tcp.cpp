/**
 * tcp.cpp
 * Project 2 - Distributed bulletin Board
 * Aparna Mahadevan
 * Ming-Hong Yang
 * Eric Kuha
 */

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include "backend.h"
#include "tcp.h"
#include <vector>

using namespace std;

struct thread_args
{ 
    int *pSocket; 
    char *type;
};

struct thread_args struct1;

int GetCoordinatorSocket(void) {
    return gCoordinatorSocket;
}

int GetServerSocket(void) {
    return gServerSocket;
}

int GetReceivingSocket(void) {
    return gReceivingSocket;
}

std::map<int, int> gSocketMap;

bool InitCoordinator(int port, char* type) {
    return InitServerWithHandler(port, server_handler, type);
}

bool InitServer(int port, char *type) {
    return InitServerWithHandler(port, client_handler, type);
}

std::map<int, int> GetMap(void) {
    return gSocketMap;
}

int RequestIndex(void) {
    char buf[MAX_LEN];

    if (gCoordinatorSocket == 0) {
        // Error?
        printf("Error?\n");
        return -1;
    }

    strcpy(buf, "getIndex;");

    SendThroughSocket(gCoordinatorSocket, buf, strlen(buf));
    memset(buf, '\0', MAX_LEN);
    RecvFromSocket(gCoordinatorSocket, buf);

    //printf("\n%s, %d\n", buf, atoi(buf));

    return atoi(buf);
}

void SendACK(int socket) {
    SendThroughSocket(socket, "ACK;", strlen("ACK;"));

    return;
}

bool InitServerWithHandler(int port, void *(*handler)(void *), char *type) {
    int serverSocket, clientSocket, c;
    struct sockaddr_in server , client;

    // Create socket
    serverSocket = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);
    if (serverSocket == -1)
    {
        printf("Could not create socket");
    }
    printf("Socket created with port %d\n", port);

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    // Bind
    if( bind(serverSocket,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        // print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    // Listen
    listen(serverSocket , 3);

    // Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    while( (clientSocket = accept(serverSocket, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        pthread_t clientThread;
        int *newSocket = (int *)malloc(sizeof(int));
        *newSocket = clientSocket;

        printf("New client connected\n");

        struct1.pSocket = newSocket;
        struct1.type = type;

        if(pthread_create(&clientThread, NULL, handler, (void *)&struct1) < 0) {
            printf("Cannot create thread\n");
            return false;
        }

        //pthread_join(clientThread, NULL);
    }

    return true;
}

bool ConnectToServer(char *serverIP, int serverPort) {
    struct sockaddr_in server;

    // Create socket for connecting to server
    gServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(gServerSocket == -1) {
        printf("Could not create socket\n");
        return false;
    }

    server.sin_addr.s_addr = inet_addr(serverIP);
    server.sin_family = AF_INET;
    server.sin_port = htons(serverPort);

    // Connect to server
    if(connect(gServerSocket, (struct sockaddr *)&server , sizeof(server)) < 0) {
        printf("Connection failed %s\n", strerror(errno));
        return false;
    }

    return true;
}

bool ConnectToCoordinator(char *serverIP, int serverPort, int localPort) {
    struct sockaddr_in server, client;
    char buf[MAX_LEN];

    // Create socket for connecting to server
    gCoordinatorSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    printf("Coordinator socket: %d\n", gCoordinatorSocket);

    if(gCoordinatorSocket == -1) {
        printf("Could not create socket\n");
        return false;
    }

    server.sin_addr.s_addr = inet_addr(serverIP);
    server.sin_family = AF_INET;
    server.sin_port = htons(serverPort);

    // Connect to coordinator
    if(connect(gCoordinatorSocket, (struct sockaddr *)&server , sizeof(server)) < 0) {
        printf("Connection failed %s\n", strerror(errno));
        return false;
    }

    // Send local port to coordinator
    sprintf(buf, "%d", localPort);
    printf("port: %s\n", buf);

    SendThroughSocket(gCoordinatorSocket, buf, strlen(buf));

    // Prepare socket for receiving messages
    int mySocket, clientSocket, c;

    // Create socket
    mySocket = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);
    if (mySocket == -1)
    {
        printf("Could not create socket");
    }
    printf("Socket created with port %d\n", localPort);

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(localPort);

    // Bind
    if( bind(mySocket,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        // print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    // Listen
    listen(mySocket , 3);

    // Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    // Only one coordinator will connect
    gReceivingSocket = accept(mySocket, (struct sockaddr *)&client, (socklen_t*)&c);

    pthread_t receivingThread;
    int *newSocket = (int *)malloc(sizeof(int));
    *newSocket = gReceivingSocket;

    if(pthread_create(&receivingThread, NULL, receiving_handler, (void *)newSocket) < 0) {
        printf("Cannot create thread\n");
        return false;
    }

    //pthread_join(receivingThread, NULL);

    return true;
}

/**
 * Handles messages sent by clients
 */
void *client_handler(void *ptr) {
    struct thread_args *args = (struct thread_args *)ptr;
    int socket = *((int *)(args->pSocket));
    int recvSize;
    char buffer[MAX_LEN];
    char *message;
    char *consType = (char *)args->type;

    while((recvSize = recv(socket, buffer, MAX_LEN, 0)) > 0) {
	buffer[recvSize] = '\0';
        message = NULL;
        printf("tcp Client hdl: %s\n", buffer);
        message = handle_request(buffer, CLIENT, consType);
        printf("Message: \n%s\n", message);
        if (message != NULL)
            send(socket, message, strlen(message), 0);
    }

    if(recvSize == 0) {
        printf("Client disconnected\n");
        fflush(stdout);
    } else if(recvSize == -1) {
        printf("Recv error");
    }

    // Free socket pointer
    free(args->pSocket);

    return NULL;
}

/**
 * Handles messages from server
 */
void *server_handler(void *ptr) {
    struct thread_args *args = (struct thread_args *) ptr;
    int socket = *((int *)(args->pSocket));
    int sendSocket = 0;
    int recvSize;
    char *message , buffer[MAX_LEN];
    char *consType = (char *) args->type;

    // Receive the port number of the client
    recvSize = recv(socket, buffer, MAX_LEN, 0);

    // Get ip from socket information
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    int res = getpeername(socket, (struct sockaddr *)&addr, &addr_size);

    // Connect back to server, the socket will be stored in gServerSocket
    ConnectToServer(inet_ntoa(addr.sin_addr), atoi(buffer));

    // Insert the socket mapping for this server
    gSocketMap.insert(std::pair<int, int>(socket, gServerSocket));

    while((recvSize = recv(socket, buffer, MAX_LEN,0)) > 0) {
        // Handle messages from servers
        buffer[MAX_LEN] = '\0';
        printf("tcp Server hdl: %s\n", buffer);
        message = NULL;
	message = handle_request(buffer, SERVER, consType);
        printf("Message: \n%s\n", message);
        if (message != NULL) {
            send(socket, message, strlen(message), 0);
            printf("SENT back\n");
        }
    }

    if(recvSize == 0) {
        printf("Server disconnected\n");
        fflush(stdout);
    } else if(recvSize == -1) {
        printf("Recv error");
    }

    // Free socket pointer
    free(args->pSocket);

    return NULL;
}

/**
 * Handles messages from Coordinator
 */
void *receiving_handler(void *pSocket) {
    int socket = *((int *)pSocket);
    int recvSize;
    char buffer[MAX_LEN];
    char *message;

    while((recvSize = recv(socket, buffer, MAX_LEN, 0)) > 0) {
        buffer[recvSize] = '\0';
        char* msg = handle_request(buffer, COORDINATOR, "");
        if(msg == NULL)
        {
            printf("Send ACK\n");
            SendACK(socket);
        }else
        {
            printf("Receiver handler Sending Message: %s\n", msg);
            SendThroughSocket(socket, msg, strlen(msg));
        }
    }

    if(recvSize == 0) {
        printf("Coordinator disconnected\n");
        fflush(stdout);
    } else if(recvSize == -1) {
        printf("Recv error");
    }

    // Free socket pointer
    free(pSocket);

    return NULL;
}

bool SendThroughSocket(int socket, char *buffer, int len) {
    usleep(1000);
    int ret = 0;

    ret = send(socket, buffer, len, 0);
    if(len == ret) {
        return true;
    } else if(ret == -1) {
        printf("Send error\n");
        return false;
    } else {
        // Size is not the same?
        return false;
    }
}

int RecvFromSocket(int socket, char *buffer) {
    return recv(socket, buffer, MAX_LEN, 0);
}

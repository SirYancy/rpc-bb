#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <map>
#include "backend.h"
#include "tcp.h"

std::map<int, int> gSocketMap;

bool InitCoordinator(int port) {
    return InitServerWithHandler(port, server_handler);
}

bool InitServer(int port) {
    return InitServerWithHandler(port, client_handler);
}

bool InitServerWithHandler(int port, void *(*handler)(void *)) {
    int serverSocket, clientSocket, c;
    struct sockaddr_in server , client;
     
    //Create socket
    serverSocket = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);
    if (serverSocket == -1)
    {
        printf("Could not create socket");
    }
    printf("Socket created with port %d\n", port);
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
     
    //Bind
    if( bind(serverSocket,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
     
    //Listen
    listen(serverSocket , 3);
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
     
    while( (clientSocket = accept(serverSocket, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        pthread_t clientThread;
        int *newSocket = &clientSocket;

        printf("New client connected\n");
         
        if(pthread_create(&clientThread, NULL, handler, (void *)newSocket) < 0) {
            printf("Cannot create thread\n");
            return false;
        }
         
        pthread_join(clientThread, NULL);
    }
     
    return true;
}

bool InitClient(char *serverIP, int serverPort, int &serverSocket) {
    struct sockaddr_in server;
     
    // Create socket for connecting to server
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(serverSocket == -1) {
        printf("Could not create socket\n");
        return false;
    }
     
    server.sin_addr.s_addr = inet_addr(serverIP);
    server.sin_family = AF_INET;
    server.sin_port = htons(serverPort);
 
    //Connect to server
    if(connect(serverSocket, (struct sockaddr *)&server , sizeof(server)) < 0) {
        printf("Connection failed\n");
        return false;
    }
    
    return true;
}

void *client_handler(void *pSocket) {
    int socket = *((int *)pSocket);
    int recvSize;
    char *message , buffer[MAX_LEN];
     
    while((recvSize = recv(socket, buffer, MAX_LEN, 0)) > 0) {
        message = handle_request(buffer);
        printf("sending: %s\n", message);

        send(socket, message, strlen(message), 0);
    }
     
    if(recvSize == 0) {
        printf("Client disconnected\n");
        fflush(stdout);
    } else if(recvSize == -1) {
        printf("Recv error");
    }
         
    // Free socket pointer
    free(pSocket);
     
    return NULL;
}

void *server_handler(void *pSocket) {
    int socket = *((int *)pSocket);
    int recvSize;
    char *message , buffer[MAX_LEN];
    
    // Receive the port number of the client
    recvSize = recv(socket, buffer, MAX_LEN, 0);

    // Insert the socket mapping for this server
    gSocketMap.insert(std::pair<int, int>(socket, atoi(buffer)));

    while((recvSize = recv(socket, buffer, MAX_LEN, 0)) > 0) {
        // Handle messages from servers
        send(socket, message, strlen(message), 0);
    }

    if(recvSize == 0) {
        printf("Client disconnected\n");
        fflush(stdout);
    } else if(recvSize == -1) {
        printf("Recv error");
    }
         
    // Free socket pointer
    free(pSocket);
     
    return NULL;
}

bool SendThroughSocket(int socket, char *buffer, int len) {
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

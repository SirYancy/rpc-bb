#include "tcp.h"

bool InitCoordinator(int port) {
}

bool InitServer(int port) {
    int serverSocket, clientSocket, socketLen;
    struct sockaddr_in server , client;
     
    //Create socket
    serverSocket = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);
    if(serverSocket == -1)
    {
        printf("Could not create socket");
    }
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
     
    // Bind socket
    if(bind(serverSocket, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        printf("Bind to socket failed\n");
        return false;
    }
     
    // Listen on socket, 3 for queueing
    listen(serverSocket , 3);
     
    // Accept and incoming connection
    socketLen = sizeof(struct sockaddr_in);
     
    while( (clientSocket = accept(serverSocket, (struct sockaddr *)&client, (socklen_t *)&socketLen)) )
    {
        pthread_t clientThread;
        int *newSocket = (int *)malloc(sizeof(int));
        *newSocket = clientSocket;

        printf("New client connected\n");
         
        if(pthread_create(&clientThread, NULL, client_handler, (void *)newSocket) < 0) {
            printf("Cannot create thread\n");
            return false;
        }
         
        //pthread_join(clientThread, NULL);
    }
     
    return true;
}

bool InitClient(char *serverIP, int serverPort, int &serverSocket) {
    struct sockaddr_in server;
    char buffer[MAX_LEN];
    int msgLen = 0;
     
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
    if(connect(serverSocket , (struct sockaddr *)&server , sizeof(server)) < 0) {
        printf("Connection failed\n");
        return false;
    }
    
    return true;
}

void *client_handler(void *pSocket)
{
    int socket = *((int *)pSocket);
    int recvSize;
    char *message , buffer[MAX_LEN];
     
    while((recvSize = recv(socket, buffer, MAX_LEN, 0)) > 0) {
        // Handle incoming message to respond
        printf("%s\n", buffer);
    }
     
    if(recvSize == 0) {
        printf("Client disconnected\n");
        fflush(stdout);
    }else if(recvSize == -1) {
        printf("Recv error");
    }
         
    // Free socket pointer
    free(pSocket);
}

bool SendThroughSocket(int socket, char *buffer, int len) {
    int ret = 0;

    ret = send(socket, buffer, sizeof(buffer), 0);
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

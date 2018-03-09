#include "tcp.h"

bool InitCoordinator(int port) {
}

bool InitServer(int port) {
    int serverSocket, clientSocket, c;
    struct sockaddr_in server , client;
     
    //Create socket
    serverSocket = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);
    if (serverSocket == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
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
        int *newSocket = malloc(1);
        *newSocket = clientSocket;

        printf("New client connected\n");
         
        if(pthread_create(&clientThread, NULL, client_handler, (void *)newSocket) < 0) {
            printf("Cannot create thread\n");
            return false;
        }
         
        pthread_join(clientThread, NULL);
    }
     
    return true;
}

bool InitClient(char *serverIP, int serverPort, int serverSocket) {
    struct sockaddr_in server;
    char buffer[MAX_LEN];
    int msgLen = 0;
     
    // Create socket for connecting to server
    sendSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sendSocket == -1) {
        printf("Could not create socket\n");
        return false;
    }
     
    server.sin_addr.s_addr = inet_addr(serverIP);
    server.sin_family = AF_INET;
    server.sin_port = htons(serverPort);

    //Bind
    if(bind(recvSocket, (struct sockaddr *)&client, sizeof(client)) < 0) {
        printf("Bind error\n");
        return false;
    }
 
    //Connect to server
    if(connect(sendSocket , (struct sockaddr *)&server , sizeof(server)) < 0) {
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
    }
     
    if(recvSize == 0) {
        printf("Client disconnected\n");
        fflush(stdout);
    }else if(recvSize == -1) {
        printf("Recv error");
    }
         
    // Free socket pointer
    free(pSocket);
     
    return;
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

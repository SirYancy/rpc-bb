#include "tcp.h"

int main(int argc, char *argv[]) {
    int serverPort = atoi(argv[2]);
    int serverSocket = 0;
    char buffer[MAX_LEN];

    // Initialize socket connection
    InitClient(argv[1], serverPort, serverSocket);

    while(true) {
        // Send something to server
        scanf("%s", buffer);
        SendThroughSocket(serverSocket, buffer, strlen(buffer));

        RecvFromSocket(serverSocket, buffer);

        printf("%s\n", buffer);
    }

    return 0;
}

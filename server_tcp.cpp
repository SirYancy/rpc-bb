#include "tcp.h"

int gCoordinatorSocket = 0;
int gServerSocket = 0;
int gReceivingSocket = 0;

int main(int argc, char *argv[]) {
    int port = atoi(argv[1]);
    char *coordinatorIP = argv[2];
    int coordinatorPort = atoi(argv[3]);
    int receivingPort = atoi(argv[4]);
    char *consistencyType = argv[5];

    printf("BB server running on port %d...\n", port);

    // Coonect to coordinator
    ConnectToCoordinator("127.0.0.1", coordinatorPort, receivingPort);

    // Initialize TCP socket
    InitServer(port, consistencyType);

    return 0;
}

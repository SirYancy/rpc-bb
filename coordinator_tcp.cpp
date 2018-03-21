#include "tcp.h"
#include "backend.h"

int gCoordinatorSocket = 0;
int gServerSocket = 0;
int gReceivingSocket = 0;

int main(int argc, char *argv[]) {
    // parameter: ./coordinator PORT policy
    int port = atoi(argv[1]);
    //int policy = atoi(argv[2]);
    char *consistencyType = argv[2];

    printf("Coordinator running on port %d...\n", port);

    // Initialize TCP socket
//    InitCoordinator(port, consistencyType);
    InitCoordinator(port, consistencyType);
    return 0;
}

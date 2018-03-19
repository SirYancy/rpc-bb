#include "tcp.h"

int gCoordinatorSocket = 0;
int gServerSocket = 0;
int gReceivingSocket = 0;

int main(int argc, char *argv[]) {
    int port = atoi(argv[1]);

    printf("BB server running on port %d...\n", port);

    // Initialize TCP socket
    InitServer(port);

    return 0;
}

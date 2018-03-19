#include "tcp.h"
#include "backend.h"

int main(int argc, char *argv[]) {
    // parameter: ./coordinator PORT policy
    int port = atoi(argv[1]);
    int policy = atoi(argv[2]);

    printf("Coordinator running on port %d...\n", port);

    // Initialize TCP socket
    InitCoordinator(port);

    return 0;
}

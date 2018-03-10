#include "tcp.h"

int main(int argc, char *argv[]) {
    int port = atoi(argv[1]);

    printf("BB server running on port %d...", port);

    // Initialize TCP socket
    InitServer(port);

    return 0;
}

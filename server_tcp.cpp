#include <iostream>
#include "tcp.h"

int gCoordinatorSocket = 0;
int gServerSocket = 0;
int gReceivingSocket = 0;

int main(int argc, char *argv[]) {

    if(argc != 6){
        std::cout << "syntax is ./server_tcp PORT COORD_IP COORD_PORT RECV_PORT CONSISTENCY TYPE (seq, quorum, rwy)" << std::endl;
        return 1;
    }


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

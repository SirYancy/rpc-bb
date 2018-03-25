#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "tcp.h"

int serverPort;

int gCoordinatorSocket = 0;
int gServerSocket = 0;
int gReceivingSocket = 0;

int main(int argc, char *argv[]) {
    clock_t start, end;
    double timedelta;

    char buffer[MAX_LEN];

    char outputFile[] = "results.csv";
    FILE *Output = NULL;

    Output = fopen(outputFile, "a");

    if(NULL != Output)
    {
        fseek(Output, 0, SEEK_END);
        int size = ftell(Output);

        if(size == 0)
        {
            fprintf(Output, "Post Number, Time\n");
        }
    }

    serverPort = atoi(argv[2]);

    ConnectToServer(argv[1], serverPort);

    int i;
    for(i = 0; i < 100; i++)
    {
        sleep(1);
        memset(buffer, '\0', MAX_LEN);
        sprintf(buffer, "post;TestUser;Title %d;Contents of article %d", i, i);
        printf("Sending post: %s\n", buffer);

        start = clock();

        SendThroughSocket(gServerSocket, buffer, strlen(buffer));
        int len = RecvFromSocket(gServerSocket, buffer);

        end = clock();

        buffer[len] = '\0';

        printf("\n%s RECEIVED\n", buffer);

        timedelta = ((double) (end - start)) / CLOCKS_PER_SEC;

        fprintf(Output, "%d, %f\n", i, timedelta);
    }

}

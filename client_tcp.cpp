#include <string.h>
#include <pthread.h>
#include "tcp.h"

int live = 1;
int serverPort;

int gCoordinatorSocket = 0;
int gServerSocket = 0;
int gReceivingSocket = 0;

void *client_thread_func(void *args);

int main(int argc, char *argv[]) {
    serverPort = atoi(argv[2]);

    pthread_t client_thread;

    // Initialize socket connection
    ConnectToServer(argv[1], serverPort);

    pthread_create(&client_thread, NULL, client_thread_func, NULL);

    while(live){}

    pthread_join(client_thread, NULL);

    return 0;
}

void *client_thread_func(void *args)
{
    char buffer[MAX_LEN];
    int menu_choice;

    char username[20];
    char title[20];
    char content[100];
    int id;

    printf("Who are you? > \n");
    scanf("%[^\n]s", username);

    while(live)
    {
	memset(buffer, '\0', MAX_LEN);
        printf("Welcome, %s!\n%s\n%s\n%s\n%s\n%s\n%s\n%s",
                username,
                "What do you want to do?",
                "1 - Post an article",
                "2 - List all articles",
                "3 - Get an article",
                "4 - Reply to an article",
                "5 - Exit",
                "> ");
        scanf("%d", &menu_choice);

        //flush input buffer
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { }
        switch(menu_choice)
        {
            case 1:
                printf("\nTitle: ");
                scanf("%[^\n]s", title);
                while ((c = getchar()) != '\n' && c != EOF) { }
                printf("\nContent: ");
                scanf("%[^\n]s", content);
                sprintf(buffer, "post;%s;%s;%s",
                    username,
                    title,
                    content);
                SendThroughSocket(gServerSocket, buffer, strlen(buffer));
                RecvFromSocket(gServerSocket, buffer);
                printf("\n%s SENT\n", buffer);
                break;
            case 2:
                printf("\nRefreshing article list:\n");
                sprintf(buffer, "list;");
                SendThroughSocket(gServerSocket, buffer, strlen(buffer));
                RecvFromSocket(gServerSocket, buffer);
                printf("\n%s\n", buffer);
                break;
            case 3:
                printf("\nWhich article: ");
                scanf("%d", &id);
                printf("\nGetting article %d\n", id);
                sprintf(buffer, "article;%d", id);
                SendThroughSocket(gServerSocket, buffer, strlen(buffer));
                RecvFromSocket(gServerSocket, buffer);
                printf("Got Article:\n %s\n", buffer);
                break;
            case 4:
                printf("\nReplying to:\n");
                scanf("%d", &id);
                while ((c = getchar()) != '\n' && c != EOF) { }
                printf("\nContent: ");
                scanf("%[^\n]s", content);
                while ((c = getchar()) != '\n' && c != EOF) { }
                sprintf(buffer, "reply;%d;%s;%s",
                    id,
                    username,
                    content);
                SendThroughSocket(gServerSocket, buffer, strlen(buffer));
                printf("%s\n", buffer);
                break;
            case 5:
                printf("exiting\n");
                live = 0;
                break;
            default:
                printf("Invalid input\n");
                break;
        }
    }
    return NULL;
}

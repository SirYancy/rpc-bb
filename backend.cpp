#include <string>
#include <iostream>
#include <map>
#include <stdio.h>
#include <string.h>
#include "Article.h" 
#include "tcp.h"
#include "backend.h"
#include <vector> 

#define PORT 8080

using namespace std;

char gBuffer[MAX_LEN];

map<int, Article*> articleMap;

Article *last;

int gIndex = 0;

char* clientHandler(char *buf);
char* serverHandler(char *buf);
void receivingHandler(char *buf);

void get_thread(Article* a, int depth);
void print_article(Article *a);
void print_list();

char* handle_request(char *command, ROLE role) {
    switch (role) {
        case CLIENT:
            return clientHandler(command);
        case SERVER:
            return serverHandler(command);
        case COORDINATOR:
            // Handle message received from coordinator
            receivingHandler(command);
            break;
        default:
            printf("Invalid role type\n");
            break;
    }
}

char* serverHandler(char *buffer) {
    char *command = strtok(buffer, ";");

    // Clear buffer
    memset(gBuffer, '\0', MAX_LEN);

    if (strcmp(command, "getIndex") == 0) {
        // Return article index
        sprintf(gBuffer, "%d", gIndex);

        // Increase index
        gIndex++;

        return gBuffer;
    } else if (strcmp(command, "post") == 0) {
        // Sync post with all the other servers

    } else if (strcmp(command, "reply") == 0) {
        // Sync reply with all the other servers
    } else if (strcmp(command, "list") == 0) {
        // 
    } else if (strcmp(command, "article") == 0) {
        //
    }
}

char* clientHandler(char *req)
{
    // Reset the buffer
    //
    memset(gBuffer, '\0', sizeof(char) * strlen(gBuffer));
    char* token;
    token = strtok(req, ";");

    char *tok1 = strtok(NULL, ";");
    char *tok2 = strtok(NULL, ";");
    char *tok3 = strtok(NULL, ";");

    char *msg;

    bool result;

    if (strcmp(token, "post") == 0)
    {
        // Request index from coordinator first
        int index = RequestIndex();

        // Send post request to coordinator
        sprintf(gBuffer, "%s;%d;", req, index);
        SendThroughSocket(GetCoordinatorSocket(), gBuffer, strlen(gBuffer));
    }
    else if (strcmp(token, "reply") == 0)
    {
        // Request index from coordinator first
        int index = RequestIndex();

        // Send post request to coordinator
        sprintf(gBuffer, "%s;%d;", req, index);
        SendThroughSocket(GetCoordinatorSocket(), gBuffer, strlen(gBuffer));
    }
    else if (strcmp(token, "list") == 0)
    {
        msg = get_list();
        printf("list %s\n", msg);
    }
    else if (strcmp(token, "article") == 0)
    {
        int id = atoi(tok1);
        msg = get_article(id);
        printf("article %s\n", msg);
    }

    return msg;
}

void receivingHandler(char *buffer) {
    char *command = strtok(buffer, ";");

    char *tok1 = strtok(NULL, ";");
    char *tok2 = strtok(NULL, ";");
    char *tok3 = strtok(NULL, ";");
    char *tok4 = strtok(NULL, ";");

    if (strcmp(command, "getIndex") == 0) {
        // Error, server should not receive this request
        return;
    } else if (strcmp(command, "post") == 0) {

        post_article(tok1, tok2, tok3, atoi(tok4));
        SendACK();

        return;
    } else if (strcmp(command, "reply") == 0) {

        post_reply(atoi(tok1), tok2, "rep", tok3, atoi(tok4));
        SendACK();

        return;
    } else if (strcmp(command, "list") == 0) {
        // 
    } else if (strcmp(command, "article") == 0) {
        //
    }
}

bool post_article(char *user, char *title, char *article, int index)
{
    printf("posting article\n");
    string u_str(user);
    string t_str(title);
    string a_str(article);
    Article *a = new Article(u_str, t_str, a_str, index);

    // Keep a reference to the most recent root article.
    if (last != NULL)
    {
        last->setNext(a);
    }
    last = a;

    articleMap.insert(make_pair(a->getID(), a));

    print_list();

    //sprintf(buffer, "%s", a->toString());

    return 1;
}

char *get_list()
{
    printf("get list\n");
    gBuffer[0] = 0;

    // Get a pointer to the first article
    Article *curr = articleMap.begin()->second;

    get_thread(curr, 0);

    printf("\nBuffer:\n%s", gBuffer);

    return gBuffer;
}

void get_thread(Article* curr, int depth)
{
    while (curr != NULL)
    {
        for(int i = 0; i < depth; i++)
        {
            sprintf(gBuffer + strlen(gBuffer), "  ");
        }
        sprintf(gBuffer + strlen(gBuffer), "%d - %s - %s - %s\n",
                curr->getID(),
                curr->getAuthor().c_str(),
                curr->getTitle().c_str(),
                curr->getContent().c_str());
        if(curr->getReply() != NULL)
        {
            get_thread(curr->getReply(), depth + 1);
        }
        curr = curr->getNext();
    }
}

char *get_article(int id)
{
    printf("get article\n");
    Article *target = articleMap.find(id)->second;

    gBuffer[0] = 0;

    sprintf(gBuffer, "%d - %s - %s\n",
            target->getID(),
            target->getAuthor().c_str(),
            target->getContent().c_str());

    return gBuffer;
}


bool post_reply(int id, char *user, char* title, char *article, int index)
{
    printf("posting reply\n");
    string u_str(user);
    string t_str(title);
    string a_str(article);
    printf("Article: %s\n", article);
    Article *a = new Article(u_str, t_str, a_str, index);

    articleMap.insert(make_pair(a->getID(), a));

    Article *target = articleMap.find(id)->second;

    Article *r = target->getReply();

    if (r == NULL)
    {
        target->setReply(a);
    }
    else
    {
        if(r->getNext() == NULL)
            r->setNext(a);
        else
        {
            r = r->getNext();
            while(r->getNext() != NULL)
            {
                r = r->getNext();
            }
            r->setNext(a);
        }
    }
    print_list();

    // TBD, remove for compiling
    //sprintf(gBuffer, "%s", a->toString());

    return 1;
}

void print_article(Article *a)
{
    cout << a->toString();
}

void print_list()
{
    for(auto const& a : articleMap)
    {
        print_article(a.second);
    }
}


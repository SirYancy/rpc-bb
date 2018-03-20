#include <string>
#include <iostream>
#include <map>
#include <stdio.h>
#include <string.h>
#include "Article.h" 
#include "tcp.h"
#include "backend.h"
#include <vector> 
#include <boost/serialization/map.hpp> 
#include <boost/archive/text_oarchive.hpp> 
#include <boost/asio.hpp> 

#define PORT 8080

using namespace std;

char gBuffer[MAX_LEN];

map<int, Article*> articleMap;

Article *last;

int gIndex = 0;

char* clientHandler(char *buf, char* type);
char* clientHandlerSeq(char *req);
char* clientHandlerQuorum(char *req);
char* clientHandlerRYW(char *req);

char* serverHandler(char *buf, char *type);
char* serverHandlerSeq(char *buffer);
char* serverHandlerQuorum(char *buffer);
char* serverHandlerRYW(char *buffer);

void receivingHandler(char *buf);

void get_thread(Article* a, int depth);
void print_article(Article *a);
void print_list();

char* handle_request(char *command, ROLE role, char *type) {
    switch (role) {
        case CLIENT:
            return clientHandler(command, type);
        case SERVER:
            return serverHandler(command, type);
        case COORDINATOR:
            // Handle message received from coordinator
            receivingHandler(command);
            break;
        default:
            printf("Invalid role type\n");
            break;
    }
}

char* serverHandler(char *buffer, char *type) {
    if (strcmp(type, "seq") == 0)
    { 
	return serverHandlerSeq(buffer);
    }
    else if (strcmp (type, "quorum") == 0)
    {
        return serverHandlerQuorum(buffer);
    }
    else if (strcmp (type, "ryw") == 0)
    { 
	return serverHandlerRYW(buffer);
    }
    else 
    { 
        printf("wrong consistency type\n");
	return NULL;    
    }
}

char* serverHandlerSeq(char* buffer)
{
    // Clear buffer
    memset(gBuffer, '\0', MAX_LEN);
    strcpy(gBuffer, buffer);

    char *command = strtok(buffer, ";");

    if (strcmp(command, "getIndex") == 0) {
        // Return article index
        sprintf(gBuffer, "%d", gIndex);

        printf("gIndex: %d\n", gIndex);
        printf("gBuffer: %s\n", gBuffer);

        // Increase index
        gIndex++;

        return gBuffer;
    } else if (strcmp(command, "post") == 0) {
        // Sync post with all the other servers
        std::map<int, int> serverMap = GetMap();
        char tmp[MAX_LEN];

        for (std::map<int, int>::iterator it = serverMap.begin(); it != serverMap.end(); it++) {
            SendThroughSocket(it->second, gBuffer, strlen(gBuffer));
            memset(tmp, '\0', MAX_LEN);
            RecvFromSocket(it->second, tmp);

            printf("%d %d %s ", it->first, it->second, tmp);

            if (strcmp(tmp, "ACK;") == 0) {
                // Go to next one;
                printf("Received ACK\n");
            }
        }

        printf("\n");

        return NULL;
    } else if (strcmp(command, "reply") == 0) {
        // Sync reply with all the other servers
        std::map<int, int> serverMap = GetMap();
        char tmp[MAX_LEN];

        for (std::map<int, int>::iterator it = serverMap.begin(); it != serverMap.end(); it++) {
            SendThroughSocket(it->second, gBuffer, strlen(gBuffer));
            memset(tmp, '\0', MAX_LEN);
            RecvFromSocket(it->second, tmp);

            printf("%d ", it->first);

            if (strcmp(tmp, "ACK;") == 0) {
                // Go to next one;
                printf("Received ACK\n");
            }
        }

        printf("\n");

        return NULL;
    } else if (strcmp(command, "list") == 0) {
        // 
    } else if (strcmp(command, "article") == 0) {
        //
    }
}

char* serverHandlerRYW(char *buffer)
{
    //handler by the coordinator to handle requests from servers 
    // Clear buffer
    memset(gBuffer, '\0', MAX_LEN);
    strcpy(gBuffer, buffer);

    char *command = strtok(buffer, ";");

    if (strcmp(command, "getIndex") == 0) {
        // Return article index
        sprintf(gBuffer, "%d", gIndex);

        printf("gIndex: %d\n", gIndex);
        printf("gBuffer: %s\n", gBuffer);

        // Increase index
        gIndex++;

        return gBuffer;
    } else if (strcmp(command, "getCopy") == 0) {
        std::map<int, int> serverMap = GetMap(); 
	std::ostringstream s;
	boost::archive::text_oarchive arch(s);
	arch << serverMap;
	string out_map = s.str();
	char *cstr = new char[out_map.length() + 1];
	strcpy(cstr, out_map.c_str());
	sprintf(gBuffer, "%s", cstr);
	return gBuffer;
    } 
    
    return NULL;
}

char* serverHandlerQuorum(char *buffer)
{
    return NULL;
}

char* clientHandler(char *req, char *type)
{ 
    if (strcmp (type, "ryw") == 0 ) {
	return clientHandlerRYW(req);
    }
    else if (strcmp (type, "quorum") == 0) { 
        return clientHandlerQuorum(req);
    }
    else if (strcmp (type, "seq") == 0) { 
	return clientHandlerSeq(req);
    }
    else { 
	printf("error in consistency type\n"); 
	return NULL; 
    }
}

char* clientHandlerSeq(char *req)
{
    // Reset the buffer
    //
    memset(gBuffer, '\0', MAX_LEN);
    printf("clientHandler: %s\n", req);
    strcpy(gBuffer, req);
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

        printf("Index: %d\n", index);

        // Send post request to coordinator
        sprintf(gBuffer, "%s;%d;", gBuffer, index);
        printf("Client hdl backend:%s\n", gBuffer);
        SendThroughSocket(GetCoordinatorSocket(), gBuffer, strlen(gBuffer));

        return NULL;
    }
    else if (strcmp(token, "reply") == 0)
    {
        // Request index from coordinator first
        int index = RequestIndex();

        // Send post request to coordinator
        sprintf(gBuffer, "%s;%d;", req, index);
        SendThroughSocket(GetCoordinatorSocket(), gBuffer, strlen(gBuffer));

        return NULL;
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

char *clientHandlerRYW(char *req)
{
    // Reset the buffer
    //
    memset(gBuffer, '\0', MAX_LEN);
    printf("clientHandler: %s\n", req);
    strcpy(gBuffer, req);
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

        printf("Index: %d\n", index);

        // Request primary copy from the coordinator
        sprintf(gBuffer, "getCopy;%s;%d;", gBuffer, index);
        printf("ClientRYW hdl backend:%s\n", gBuffer);
        SendThroughSocket(GetCoordinatorSocket(), gBuffer, strlen(gBuffer));

        return NULL;
    }


	return NULL;
}

char *clientHandlerQuorum(char *req){ 
	return NULL;
}
void receivingHandler(char *buffer) {
    printf("receiving hdl: %s\n", buffer);
    char *command = strtok(buffer, ";");

    char *tok1 = strtok(NULL, ";");
    char *tok2 = strtok(NULL, ";");
    char *tok3 = strtok(NULL, ";");
    char *tok4 = strtok(NULL, ";");

    if (strcmp(command, "getIndex") == 0) {
        // Error, server should not receive this request
        return;
    } else if (strcmp(command, "post") == 0) {
        printf("Received post request from coordinator\n");

        post_article(tok1, tok2, tok3, atoi(tok4));

        return;
    } else if (strcmp(command, "reply") == 0) {
        printf("Received reply request from coordinator\n");

        post_reply(atoi(tok1), tok2, "rep", tok3, atoi(tok4));

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
    gBuffer[0] = '\0';

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


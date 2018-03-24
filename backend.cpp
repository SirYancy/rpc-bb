#include <string>
#include <algorithm>
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

int gIndex = 1;

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
/*    //handler by the coordinator to handle requests from servers 
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
  */  
    return NULL;
}

char* serverHandlerQuorum(char *buffer)
{
    // Clear buffer
    memset(gBuffer, '\0', MAX_LEN);
    strcpy(gBuffer, buffer);

    // Get the server map
    std::map<int, int> serverMap = GetMap();

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
        int n = serverMap.size();
        int m = (n + 2 - 1) / 2;

        vector<int> keys = getQuorum(m);

        // Sync post with all the other servers
        char tmp[MAX_LEN];

        for (auto const& i: keys)
        {
            map<int,int>::iterator it = serverMap.find(i);

            syncServer(it->second);

            //SendThroughSocket(s, gBuffer, strlen(gBuffer));
            memset(tmp, '\0', MAX_LEN);
            //RecvFromSocket(s, tmp);

            //printf("%d %d %s ", i, s, tmp);

            if (strcmp(tmp, "ACK;") == 0) {
                // Go to next one;
                printf("Received ACK\n");
            }
        }

        printf("\n");

        return NULL;
    }
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
        sprintf(gBuffer, "%s;%d;", gBuffer, index);
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
    /*// Reset the buffer
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

*/
	return NULL;
}

char *clientHandlerQuorum(char *req){ 
    // Reset the buffer
    //
    memset(gBuffer, '\0', MAX_LEN);
    printf("clientHandler: %s\n", req);
    strcpy(gBuffer, req);
    char* token;
    token = strtok(req, ";");
    
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
    return NULL;
}
void receivingHandler(char *buffer) {
    printf("receiving hdl: %s\n", buffer);
    char *command = strtok(buffer, ";");

    char *tok1 = strtok(NULL, ";");
    char *tok2 = strtok(NULL, ";");
    char *tok3 = strtok(NULL, ";");
    char *tok4 = strtok(NULL, ";");
    char *tok5 = strtok(NULL, ";");

    if (strcmp(command, "getIndex") == 0) {
        // Coordinator wants this server's current index/version
        printf("Received index request from server: %d\n", gIndex);
        memset(gBuffer, '\0', MAX_LEN);
        sprintf(gBuffer, "index;%d", gIndex);
        SendThroughSocket(GetCoordinatorSocket(), gBuffer, strlen(gBuffer));
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
    } else if (strcmp(command, "sync") == 0) {
        int targetID = atoi(tok5);

        if(targetID == 0)
        {
            post_article(tok1, tok2, tok3, atoi(tok4));
        }
        else
        {
            post_reply(targetID, tok1, tok2, tok3, atoi(tok4));
        }



        
    }
}

bool post_article(char *user, char *title, char *article, int index)
{
    printf("posting article\n");
    string u_str(user);
    string t_str(title);
    string a_str(article);

    Article *a = new Article(u_str, t_str, a_str, index);
    a->setParentID(0);

    // Keep a reference to the most recent root article.
    if (last != NULL)
    {
        last->setNext(a);
    }
    last = a;

    articleMap.insert(make_pair(a->getID(), a));

    print_list();

    gIndex = index;

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

    memset(gBuffer, '\0', MAX_LEN);
    //gBuffer[0] = 0;

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
    a->setParentID(target->getID());

    Article *r = target->getReply();

    if (r == NULL)
    {
        target->setReply(a);
    }
    else
    {
        if(r->getNext() != NULL)
        {
            r = r->getNext();
            while(r->getNext() != NULL)
            {
                r = r->getNext();
            }
        }
        r->setNext(a);
    }
    print_list();

    gIndex = index;

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

vector<int> getQuorum(int num)
{
    // Select num random servers
    std::map<int, int> serverMap = GetMap();

    std::vector<int> indices;
    for (int i = 0; i < serverMap.size(); i++) indices.push_back(i);

    std::random_shuffle( indices.begin(), indices.end() );
    indices.resize(num);

    std::vector<int> keys;

    for (auto const& i: indices)
    {
        std::map<int,int>::iterator item = serverMap.begin();
        std::advance(item, i);

        keys.push_back(item->first);
    }


    return keys;
}

void syncServer(int socket)
{
    char *getIndex = "getIndex;";
    char *syncCommand = "sync;";
    char cIndex[10];
    memset(gBuffer, '\0', strlen(cIndex));
    SendThroughSocket(socket, getIndex, strlen(getIndex));
    RecvFromSocket(socket, cIndex);

    int index = atoi(strtok(cIndex, ";"));
    if(index < gIndex)
    {
        index++;
        for(index; index <= gIndex; index++)
        {
            memset(gBuffer, '\0', strlen(cIndex));
            Article *a = articleMap.find(index)->second;
            sprintf(gBuffer, "%s%s",
                    syncCommand,
                    a->getSyncString());
            SendThroughSocket(socket, getIndex, strlen(getIndex));
        }
    }
    
    return;
}

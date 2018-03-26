#include <string>
#include <algorithm>
#include <iostream>
#include <map>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <vector> 
#include <random>
#include "Article.h" 
#include "tcp.h"
#include "backend.h"

#define PORT 8080

using namespace std;

char gBuffer[MAX_LEN];

map<int, Article*> articleMap;

Article *last;

int gIndex = 0;

auto rng = std::default_random_engine {};

char* handle_request(char *command, ROLE role, char *type) {
    switch (role) {
        case CLIENT:
            // Server handles a message from a client
            return clientHandler(command, type);
        case SERVER:
            // Coordinator handles a message from a server
            return serverHandler(command, type);
        case COORDINATOR:
            // Server handles a message from the coordinator
            return receivingHandler(command);
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
    return serverHandlerSeq(buffer);
}

char* serverHandlerQuorum(char *buffer)
{
    // Clear buffer
    memset(gBuffer, '\0', MAX_LEN);
    strcpy(gBuffer, buffer);

    // Get the server map
    std::map<int, int> serverMap = GetMap();

    char *command = strtok(buffer, ";");
    char *tok1 = strtok(NULL, ";");
    char *tok2 = strtok(NULL, ";");
    char *tok3 = strtok(NULL, ";");
    char *tok4 = strtok(NULL, ";");

    if (strcmp(command, "getIndex") == 0) {
        printf("Server requested new index\n");

        // Increase index
        gIndex++;

        // Return article index
        sprintf(gBuffer, "%d", gIndex);

        printf("gIndex: %d\n", gIndex);
        printf("gBuffer: %s\n", gBuffer);

        printf("New gIndex: %d\n", gIndex);

        return gBuffer;
    } else if (strcmp(command, "post") == 0) {
        char tmp[MAX_LEN];
        memset(tmp, '\0', MAX_LEN);

        printf("POSTING\n");
        int n = serverMap.size();
        int m = (n + 1) / 2;

        // Add one more server if the number of servers is even
        if(n % 2 == 0)
            m++;

        vector<int> keys = getQuorum(m);
        // Most recent index
        int leadSocket = getLead(&keys);
        printf("Lead Socket: %d\n", leadSocket);

        // Post the article at lead server first
        SendThroughSocket(leadSocket, gBuffer, strlen(gBuffer));
        RecvFromSocket(leadSocket, tmp);

        if (strcmp(tmp, "ACK;") == 0) {
            // Go to next one;
            printf("Received ACK\n");
        }

        // Sync post with all the other servers in quorum
        for (auto const& i: keys)
        {
            if(i != leadSocket)
            {
                map<int,int>::iterator it = serverMap.find(i);

                syncServer(leadSocket, it->second);
            }
        }

        printf("\n");

        return "ACK";
    } else if (strcmp(command, "reply") == 0) {
        char tmp[MAX_LEN];
        memset(tmp, '\0', MAX_LEN);

        printf("REPLYING\n");
        int n = serverMap.size();
        int m = (n + 1) / 2;

        // Add one more server if the number of servers is even
        if(n % 2 == 0)
            m++;

        vector<int> keys = getQuorum(m);
        // Most recent index
        int leadSocket = getLead(&keys);
        printf("Lead Socket: %d\n", leadSocket);

        // Post the article at lead server first
        SendThroughSocket(leadSocket, gBuffer, strlen(gBuffer));
        RecvFromSocket(leadSocket, tmp);
        if (strcmp(tmp, "ACK;") == 0) {
            // Go to next one;
            printf("Received ACK\n");
        }

        // Sync post with all the other servers in quorum
        for (auto const& i: keys)
        {
            if(i != leadSocket)
            {
                map<int,int>::iterator it = serverMap.find(i);

                syncServer(leadSocket, it->second);
            }
        }

        printf("\n");

        return NULL;
    } else if (strcmp(command, "list") == 0) {
        printf("LISTING\n");

        int n = serverMap.size();
        int m = (n + 1) / 2;

        vector<int> keys = getQuorum(m);

        // Get the most up-to-date server
        int leadSocket = getLead(&keys);
        printf("Lead Socket: %d\n", leadSocket);

        // Get the list from the lead server
        SendThroughSocket(leadSocket, gBuffer, strlen(gBuffer));

        int len = RecvFromSocket(leadSocket, gBuffer);
        gBuffer[len] = '\0';

        return gBuffer;

    } else if (strcmp(command, "article") == 0) {
        printf("ARTICLE\n");

        int n = serverMap.size();
        int m = (n + 1) / 2;

        vector<int> keys = getQuorum(m);

        // Get the most up-to-date server
        int leadSocket = getLead(&keys);
        printf("Lead Socket: %d\n", leadSocket);

        // Get the list from the lead server
        SendThroughSocket(leadSocket, gBuffer, strlen(gBuffer));

        int len = RecvFromSocket(leadSocket, gBuffer);
        gBuffer[len] = '\0';

        return gBuffer;
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
    printf("client handler RYW called\n");
    // Reset the buffer
    //
    memset(gBuffer, '\0', MAX_LEN);
    printf("clientHandler: %s\n", req);
    strcpy(gBuffer, req);
    printf("gBuffer is %s\n", gBuffer);
    char* token;
    token = strtok(req, ";");

    char *tok1 = strtok(NULL, ";");
    char *tok2 = strtok(NULL, ";");
    char *tok3 = strtok(NULL, ";");
    char *tok4 = strtok(NULL, ";");

    char *msg;

    bool result;

    if (strcmp(token, "post") == 0)
    {
        // Request index from coordinator first
        // coord_flag = 1;
        int index = RequestIndex();

        printf("Index: %d\n", index);
        //update your local copy of the article map	
        post_article(tok1, tok2, tok3, index);
     
        //tell coordinator to propagate updates to everyone else 
        sprintf(gBuffer, "%s;%d;", gBuffer, index);
        printf("ClientRYW hdl backend:%s\n", gBuffer);
        SendThroughSocket(GetCoordinatorSocket(), gBuffer, strlen(gBuffer));

        return NULL;
    }
    else if (strcmp(token, "reply") == 0)
    { 
	int index = RequestIndex(); 
	post_reply(atoi(tok1), tok2, "rep", tok3, index);
        sprintf(gBuffer, "%s;%d;", gBuffer, index);
	printf("ClientRYW Reply: %s\n", gBuffer);
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

char *clientHandlerQuorum(char *req){ 
    // Reset the buffer
    //
    memset(gBuffer, '\0', MAX_LEN);
    printf("clientHandler: %s\n", req);
    strcpy(gBuffer, req);
    char* command;
    command = strtok(req, ";");

    char *msg;

    bool result;

    if (strcmp(command, "post") == 0)
    {
        printf("POSTING: Requestiong Index from Coordinator\n");
        // Request index from coordinator first
        // coord_flag = 1;
        int index = RequestIndex();

        printf("Index: %d\n", index);

        // Send post request to coordinator
        sprintf(gBuffer, "%s;%d;", gBuffer, index);
        printf("Client hdl backend:%s\n", gBuffer);
        SendThroughSocket(GetCoordinatorSocket(), gBuffer, strlen(gBuffer));

        int len = RecvFromSocket(GetCoordinatorSocket(), gBuffer);
        gBuffer[len] = '\0';

        return gBuffer;

    } else if (strcmp(command, "reply") == 0) {
        printf("REPLYING: Requesting Index from Coordinator\n");

        int index = RequestIndex();

        printf("Index: %d\n", index);

        sprintf(gBuffer, "%s;%d;", gBuffer, index);
        printf("Client hdl backend:%s\n", gBuffer);
        SendThroughSocket(GetCoordinatorSocket(), gBuffer, strlen(gBuffer));

        return NULL;

    } else if (strcmp(command, "list") == 0) {
        printf("LISTING: Requesting all articles from coordinator");
        SendThroughSocket(GetCoordinatorSocket(), gBuffer, strlen(gBuffer));
        int len = RecvFromSocket(GetCoordinatorSocket(), gBuffer);

        return gBuffer;
    } else if (strcmp(command, "article") == 0) {
        printf("ARTICLE: Geting an article\n");

        SendThroughSocket(GetCoordinatorSocket(), gBuffer, strlen(gBuffer));
        int len = RecvFromSocket(GetCoordinatorSocket(), gBuffer);

        return gBuffer;
    }
    return NULL;
}

char *receivingHandler(char *buffer) {
    printf("receiving hdl: %s\n", buffer);
    char *command = strtok(buffer, ";");

    char *tok1 = strtok(NULL, ";");
    char *tok2 = strtok(NULL, ";");
    char *tok3 = strtok(NULL, ";");
    char *tok4 = strtok(NULL, ";");
    char *tok5 = strtok(NULL, ";");

    if (strcmp(command, "queryIndex") == 0) {
        // Coordinator wants this server's current index/version
        printf("Received index request from Coordinator: %d\n", gIndex);
        memset(gBuffer, '\0', MAX_LEN);
        sprintf(gBuffer, "index;%d", gIndex);
        return gBuffer;
    } else if (strcmp(command, "post") == 0) {
        printf("Received post request from coordinator\n");

        post_article(tok1, tok2, tok3, atoi(tok4));

    } else if (strcmp(command, "reply") == 0) {
        printf("Received reply request from coordinator\n");

        post_reply(atoi(tok1), tok2, "re:", tok3, atoi(tok4));

    } else if (strcmp(command, "list") == 0) {
        printf("Received list request from coordinator\n");

        return get_list();

    } else if (strcmp(command, "article") == 0) {
        printf("Received article request from coordinator\n");

        return get_article(atoi(tok1));

    } else if (strcmp(command, "sync") == 0) {
        // For quorum Consistency. Used to sync servers
        int targetID = atoi(tok5);

        if(targetID == 0)
        {
            post_article(tok1, tok2, tok3, atoi(tok4));
        }
        else
        {
            post_reply(targetID, tok1, tok2, tok3, atoi(tok4));
        }
    } else if (strcmp(command, "syncarticle") == 0) {
        // For quorum Consistency. Used to sync a specific article
        int i = atoi(tok1);
        Article*a = articleMap.find(i)->second;

        memset(gBuffer, '\0', MAX_LEN);
        sprintf(gBuffer, "%s", a->getSyncString().c_str());
        return gBuffer;
    }

    return NULL;
}

bool post_article(char *user, char *title, char *article, int index)
{
    printf("posting article\n");
    string u_str(user);
    string t_str(title);
    string a_str(article);
    if (articleMap.find(index) != articleMap.end())
    {
        printf("Already posted this article\n");
	return 1;
    }
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

    sprintf(gBuffer, "%d - %s - %s - %s\n",
            target->getID(),
            target->getAuthor().c_str(),
            target->getTitle().c_str(),
            target->getContent().c_str());

    printf("gBuffer %s\n", gBuffer);
    return gBuffer;
}


bool post_reply(int id, char *user, char* title, char *article, int index)
{
    printf("posting reply\n");
    string u_str(user);
    string t_str(title);
    string a_str(article);
    printf("Article: %s\n", article);
    if (articleMap.find(index) != articleMap.end())
    {
        printf("Already posted this reply\n");
        return 1;
    }
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

    std::shuffle(std::begin(indices), std::end(indices), rng);
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

char *queryIndex = "queryIndex;";
char *syncCommand = "sync;";
char cIndex[10];

/**
 * Returns socket of lead server
 */
int getLead(vector<int> *keys)
{
    printf("Getting Most Updated Quorum Member\n");
    printf("Current Index: %d\n", gIndex);

    std::map<int, int> serverMap = GetMap();

    int leadSocket = -1;
    int index = -1;

    for(auto const& i: *keys)
    {
        map<int,int>::iterator it = serverMap.find(i);
        int nIndex;

        memset(cIndex, '\0', 10);
        SendThroughSocket(it->second, queryIndex, strlen(queryIndex));
        RecvFromSocket(it->second, cIndex);
        char *cmd = strtok(cIndex, ";");

        if(strcmp(cmd, "index") == 0)
        {
            nIndex = atoi(strtok(NULL, ";"));
            printf("Test Index: %d\n", nIndex);
        }else{
            printf("ERROR: Didn't send the right code back\n");
            return NULL;
        }

        if (nIndex > index)
        {
            index = nIndex;
            leadSocket = it->second;
        }
    }

    return leadSocket;
}


void syncServer(int leadSocket, int syncSocket)
{
    printf("Syncing Server\n");
    char tmp[MAX_LEN];
    char article[MAX_LEN];
    memset(gBuffer, '\0', MAX_LEN);
    int leadIndex, syncIndex;
    char *cmd;

    // Get Lead index
    memset(cIndex, '\0', 10);
    SendThroughSocket(leadSocket, queryIndex, strlen(queryIndex));
    RecvFromSocket(leadSocket, cIndex);
    cmd = strtok(cIndex, ";");

    if(strcmp(cmd, "index") == 0)
    {
        leadIndex = atoi(strtok(NULL, ";"));
        printf("Received leadIndex: %d\n", leadIndex);
    }else{
        printf("ERROR: Didn't send the right code back\n");
        return;
    }

    // Get target index
    memset(cIndex, '\0', 10);
    SendThroughSocket(syncSocket, queryIndex, strlen(queryIndex));
    RecvFromSocket(syncSocket, cIndex);
    cmd = strtok(cIndex, ";");
    if(strcmp(cmd, "index") == 0)
    {
        syncIndex = atoi(strtok(NULL, ";"));
        printf("Received syncIndex: %d\n", syncIndex);
    }else{
        printf("ERROR: Didn't send the right code back\n");
        return;
    }

    // Sync Servers if necessary
    if(syncIndex < leadIndex)
    {
        printf("Syncing Servers\n");
        // increment by one to get next article
        for(syncIndex; syncIndex <= leadIndex; syncIndex++)
        {
            memset(gBuffer, '\0', MAX_LEN);
            memset(tmp, '\0', MAX_LEN);
            memset(article, '\0', MAX_LEN);

            // Get the next article
            sprintf(gBuffer, "syncarticle;%d", syncIndex);
            SendThroughSocket(leadSocket, gBuffer, strlen(gBuffer));
            int recvlen = RecvFromSocket(leadSocket, article);
            article[recvlen] = '\0';

            printf("Article to sync: %s\n", gBuffer);
            sprintf(gBuffer, "%s%s",
                    syncCommand,
                    article);

            printf("Sending sync command: %s\n", gBuffer);

            // Send it to the sync target
            SendThroughSocket(syncSocket, gBuffer, strlen(gBuffer));
            RecvFromSocket(syncSocket, tmp);
            if (strcmp(tmp, "ACK;") == 0) {
                // Go to next one;
                printf("Received ACK\n");
            }
        }
    }
}

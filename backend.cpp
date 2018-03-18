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

char buffer[1024];

map<int, Article*> articleMap;

Article *last;

void get_thread(Article* a, int depth);
void print_article(Article *a);
void print_list();

char *handle_request(char *req)
{
    // Reset the buffer
    //
    memset(buffer, '\0', sizeof(char) * strlen(buffer));
    char* token;
    token = strtok(req, ";");

    char *tok1 = strtok(NULL, ";");
    char *tok2 = strtok(NULL, ";");
    char *tok3 = strtok(NULL, ";");

    char *msg;

    bool result;

    if (strcmp(token, "post") == 0)
    {
        result = post_article(tok1, tok2, tok3);
    }
    else if (strcmp(token, "reply") == 0)
    {	
        int id = atoi(tok1); 
        result = post_reply(id, tok2, "rep", tok3); 
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

    return buffer;
}

bool post_article(char *user, char *title, char *article)
{
    printf("posting article\n");
    string u_str(user);
    string t_str(title);
    string a_str(article);
    Article *a = new Article(u_str, t_str, a_str);

    // Keep a reference to the most recent root article.
    if (last != NULL)
    {
        last->setNext(a);
    }
    last = a;

    articleMap.insert(make_pair(a->getID(), a));

    print_list();

    sprintf(buffer, "%s", a->toString());

    return 1;
}

char *get_list()
{
    printf("get list\n");
    buffer[0] = 0;

    // Get a pointer to the first article
    Article *curr = articleMap.begin()->second;

    get_thread(curr, 0);

    printf("\nBuffer:\n%s", buffer);

    return buffer;
}

void get_thread(Article* curr, int depth)
{
    while (curr != NULL)
    {
        for(int i = 0; i < depth; i++)
        {
            sprintf(buffer + strlen(buffer), "  ");
        }
        sprintf(buffer + strlen(buffer), "%d - %s - %s - %s\n",
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

    buffer[0] = 0;

    sprintf(buffer, "%d - %s - %s\n",
            target->getID(),
            target->getAuthor().c_str(),
            target->getContent().c_str());

    return buffer;
}


bool post_reply(int id, char *user, char* title, char *article)
{
    printf("posting reply\n");
    string u_str(user);
    string t_str(title);
    string a_str(article);
    printf("Article: %s\n", article);
    Article *a = new Article(u_str, t_str, a_str);

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

    sprintf(buffer, "%s", a->toString());

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


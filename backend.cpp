#include <string>
#include <iostream>
#include <map>
#include <stdio.h>
#include <string.h>
#include "Article.h" 
#include "tcp.h"
#include <vector> 

#define PORT 8080

using namespace std;

char buffer[1000];

map<int, Article*> articleMap;

Article *last;

void get_thread(Article* a, int depth);
void print_article(Article *a);
void print_list();

char **handle_request(char *req)
{
    char** msg = (char **) malloc (4*sizeof(char*));
    for (int s = 0; s < 4; s++)
    {
        msg[s] = (char*) malloc(100*sizeof(char));
    }
    int i = 0;
    char* token;
    token = strtok(req, ";");
    while(token != NULL) {
        printf(" %s\n", token);
        msg[i] = token;
        i++;
        token = strtok(NULL, ";");
    }
    return msg;
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

    return 1;
}

char *get_list()
{
    printf("get list\n");
    buffer[0] = 0;

    // Get a pointer to the first article
    Article *curr = articleMap.begin()->second;

    get_thread(curr, 0);

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


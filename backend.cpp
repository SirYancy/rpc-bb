#include <string>
#include <iostream>
#include <map>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "backend.h"
#include "Article.h" 
#include "tcp.h"

using namespace std;

char buffer[1000];

map<int, Article*> articleMap;

Article *last;

void get_thread(Article* a, int depth);
void print_article(Article *a);
void print_list();

char *handle_request(char *req)
{
    char *token;
    token = strtok(req, ";");

    if (!strcmp(token, "post"))
    {
        printf("posting an article\n");
        char *user = strtok(NULL, ";");
        char *title = strtok(NULL, ";");
        char *content = strtok(NULL, ";");

        post_article(user, title, content);

        sprintf(buffer, "done\n");
        return buffer;
    }
    else if (!strcmp(token, "list"))
    {
        printf("Getting List\n");
        return get_list();
    }
    else if (!strcmp(token, "article"))
    {
        printf("Getting article\n");
        int id = atoi(strtok(NULL, ";"));
        return get_article(id);
    }
    else if (!strcmp(token, "reply"))
    {
        printf("Replying\n");
        int id = atoi(strtok(NULL, ";"));
        char *user = strtok(NULL, ";");
        char *content = strtok(NULL, ";");
        post_reply(id, user, content);
        sprintf(buffer, "done\n");
        return buffer;
    }
    else
    {
        printf("Invalid request\n");
        return NULL;
    }

}

bool post_article(char *user, char *title, char *article)
{
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
    Article *target = articleMap.find(id)->second;

    buffer[0] = 0;

    sprintf(buffer, "%d - %s - %s - %s\n",
            target->getID(),
            target->getAuthor().c_str(),
            target->getTitle().c_str(),
            target->getContent().c_str());

    return buffer;
}


bool post_reply(int id, char *user, char *article)
{
    //get target article
    Article *target = articleMap.find(id)->second;

    string t_str = target->getTitle();

    string u_str(user);
    string a_str(article);
    Article *a = new Article(u_str, t_str, a_str);

    articleMap.insert(make_pair(a->getID(), a));

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


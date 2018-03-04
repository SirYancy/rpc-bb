#include <string>
#include <iostream>
#include <map>
#include <string.h>
#include "bb.h"
#include "backend.h"
#include "Article.h"

using namespace std;

char buffer[1000];

map<int, Article*> articleMap;

Article *last;

void get_thread(Article* a, int depth);
void print_article(Article *a);
void print_list();

bool_t post_article(char *user, char *article)
{
    string u_str(user);
    string a_str(article);
    Article *a = new Article(u_str, a_str);

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
        sprintf(buffer + strlen(buffer), "%d - %s - %s\n",
                curr->getID(),
                curr->getAuthor().c_str(),
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
    return NULL;

}

/**
 * TODO - Error checking
 */
bool_t post_reply(int id, char *user, char *article)
{
    string u_str(user);
    string a_str(article);
    Article *a = new Article(u_str, a_str);

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

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

char *get_sub_thread(Article *a);
void print_article(Article *a);
void print_list();

bool_t post_article(char *user, char *article)
{
    string u_str(user);
    string a_str(article);
    Article *a = new Article(u_str, a_str);

    articleMap.insert(make_pair(a->getID(), a));

    print_list();

    return 1;
}

char *get_list()
{
    /**
    buffer[0] = 0;
    Article *curr = articleList;
    printf("DEBUG - Article: %s\n", curr->article);

    while (curr != NULL)
    {
        printf("DEBUG - Article: %s\n", curr->article);
        sprintf(buffer, "%d ", curr->id);
        sprintf(buffer, " - %s: ", curr->author);
        sprintf(buffer, "%s\n", curr->article);
        curr = curr->next;
    }

    return buffer;
    */
}

char *get_article(int id)
{

}

bool_t post_reply(int id, char *user, char *article)
{

}

/*
char *get_sub_thread(Article *a)
{

}
*/

/*
void print_article(Article *a)
{
    cout << a->toString();
}
*/

void print_list()
{
    /*
    Article *curr = articleList;

    while (curr != NULL)
    {
        print_article(curr);
        curr = curr->next;
    }
    */

}

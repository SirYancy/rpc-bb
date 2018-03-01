#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bb.h"
#include "backend.h"

char buffer[1000];

static Article *articleList;
static Article *lastArticle;
static int articleCount = 0;

char *get_sub_thread(Article *a);
void print_article(Article *a);
void print_list();

bool_t post_article(char *user, char *article)
{
    Article *a = (Article *) malloc(sizeof(Article));

    articleCount++;
    a->id = articleCount;
    a->author = user;
    a->article = article;
    a->next = NULL;
    a->reply = NULL;

    if(articleCount == 1)
    {
        articleList = a;
        lastArticle = a;
    }
    else
    {
        lastArticle->next = a;
        lastArticle = a;
    }

    print_list();

    return 1;
}

char *get_list()
{
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
}

char *get_article(int id)
{

}
bool_t post_reply(int id, char *user, char *article)
{

}

char *get_sub_thread(Article *a)
{

}

void print_article(Article *a)
{
    printf("%d - %s - %s\n", a->id, a->author, a->article);
}

void print_list()
{
    Article *curr = articleList;

    while (curr != NULL)
    {
        print_article(curr);
        curr = curr->next;
    }

}

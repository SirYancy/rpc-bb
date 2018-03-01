#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "backend.h"

static Article *articleList;
static Article *lastArticle;
static int articleCount = 0;

bool_t post_article(char *user, char *article)
{
    Article *a = (Article *) malloc(sizeof(Article));

    articleCount++;
    a->id = articleCount;
    a->author = user;
    a->article = article;

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

    return 1;
}

char *get_list()
{

}
char *get_article(int id)
{

}
bool_t post_reply(int id, char *user, char *article)
{

}

#ifndef BACKEND
#define BACKEND

typedef struct Article
{
    int id;
    char *article;
    char *author;
    struct Article *next;
    struct Article *reply;
} Article;


bool_t post_article(char *user, char *article);
char *get_list();
char *get_article(int id);
bool_t post_reply(int id, char *user, char *article);

#endif /* BACKEND */

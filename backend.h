#ifndef BACKEND
#define BACKEND
#include "bb.h"
#include "Article.h"

bool_t post_article(char *user, char *contents);
char *get_list();
char *get_article(int id);
bool_t post_reply(int id, char *user, char *contents);

#endif /* BACKEND */

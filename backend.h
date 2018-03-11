#ifndef BACKEND_H
#define BACKEND_H

char *handle_request(char *req);
bool post_article(char *user, char *title, char *article);
char *get_list();
char *get_article(int id);
bool post_reply(int id, char *user, char *article);

#endif //BACKEND_H

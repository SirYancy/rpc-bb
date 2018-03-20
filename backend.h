#include <vector>
#ifndef BACKEND_H
#define BACKEND_H
enum ROLE {CLIENT, SERVER, COORDINATOR};

char* handle_request(char *req, ROLE role, char *type);
bool post_article(char *user, char *title, char *article, int index);
char *get_list();
char *get_article(int id);
bool post_reply(int id, char *user, char *title, char *article, int index);

#endif //BACKEND_H

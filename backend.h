/**
 * backend.h
 * Project 2 - Distributed bulletin Board
 * Aparna Mahadevan
 * Ming-Hong Yang
 * Eric Kuha
 */

#include "Article.h"
#include <vector>
#ifndef BACKEND_H
#define BACKEND_H
enum ROLE {CLIENT, SERVER, COORDINATOR};

char* clientHandler(char *buf, char* type);
char* clientHandlerSeq(char *req);
char* clientHandlerQuorum(char *req);
char* clientHandlerRYW(char *req);

char* serverHandler(char *buf, char *type);
char* serverHandlerSeq(char *buffer);
char* serverHandlerQuorum(char *buffer);
char* serverHandlerRYW(char *buffer);

char* receivingHandler(char *buf);

void get_thread(Article* a, int depth);
void print_article(Article *a);
void print_list();

vector<int> getQuorum(int num);
int getLead(vector<int> *keys);
void syncServer(int leadSocket, int syncSocket);

char* handle_request(char *req, ROLE role, char *type);
bool post_article(char *user, char *title, char *article, int index);
char *get_list();
char *get_article(int id);
bool post_reply(int id, char *user, char *title, char *article, int index);

#endif //BACKEND_H

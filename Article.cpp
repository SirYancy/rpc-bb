#include <string>
#include <sstream>
#include <iostream>

#include "Article.h"

using namespace std;

int Article::articleCount = 0;


/**
 * Constructor
 */
Article::Article(const string a, const string c)
    :id(++articleCount), author(a), contents(c), next(NULL), reply(NULL)
{}

/**
 * Destructor
 */
Article::~Article()
{}

/**
 * get the id of the article
 */
int Article::getID() const
{
    return id;
}

/**
 * get the author of the article
 */
string Article::getAuthor() const
{
    return author;
}

/**
 * get the contents of the article
 */
string Article::getContent() const
{
    return contents;
}

/**
 * Setter for next article
 */
void Article::setNext(Article *a)
{
    next = a;
}

/**
 * Getterfor next article
 */
Article *Article::getNext() const
{
    return next;
}

/**
 * Setter for reply article
 */
void Article::setReply(Article *a)
{
    reply = a;
}

/**
 * Getter for reply article
 */
Article *Article::getReply() const
{
    return reply;
}

/**
 * returns a string representation of the article
 */
string Article::toString() const
{
    ostringstream output;

    output << endl
        << id << " - "
        << author << " - "
        << contents << endl;

    return output.str();
}

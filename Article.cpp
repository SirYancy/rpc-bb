/**
 * Article.cpp
 * Project 2 - Distributed bulletin Board
 * Aparna Mahadevan
 * Ming-Hong Yang
 * Eric Kuha
 */

#include <string>
#include <sstream>
#include <iostream>

#include "Article.h"

using namespace std;

/**
 * Constructor
 */
Article::Article(const string a, const string t, const string c, int index)
    :id(index), author(a), title(t), contents(c), next(NULL), reply(NULL)
{}

/**
 * get the id of the article
 */
int Article::getID() const
{
    return id;
}

/**
 * Get the id of the article's parent
 */
int Article::getParentID() const
{
    return parentId;
}

/**
 * Set parent ID
 */
void Article::setParentID(int i)
{
    parentId = i;
}

/**
 * get the author of the article
 */
string Article::getAuthor() const
{
    return author;
}

/**
 * Get the title of the article
 */
string Article::getTitle() const
{
    return title;
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
        << title << " - "
        << contents << endl;

    return output.str();
}

/**
 * Formatted string for syncing servers
 */
string Article::getSyncString() const
{
    ostringstream output;

    output 
        << author << ";"
        << title << ";"
        << contents << ";"
        << id << ";"
        << parentId;

    return output.str();
}

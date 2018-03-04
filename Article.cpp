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
    :id(++articleCount), author(a), contents(c)
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
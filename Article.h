/**
 * Article.h
 * Project 2 - Distributed bulletin Board
 * Aparna Mahadevan
 * Ming-Hong Yang
 * Eric Kuha
 */

#ifndef ARTICLE_H
#define ARTICLE_H

#include <string>

using namespace std;

class Article{
    private: 
        const int id;
        int parentId;
        string author;
        string title;
        string contents;

        Article *next;
        Article *reply;

    public:
        static int articleCount;

        /**
         * Constructor
         */
        Article(const string, const string, const string, int);
        
        /**
         * get the id of the article
         */
        int getID() const;

        /**
         * Get the id of the article's parent
         */
        int getParentID() const;

        /**
         * Set parent ID
         */
        void setParentID(int);

        /**
         * get the author of the article
         */
        string getAuthor() const;

        /**
         * Get the title of the article
         */
        string getTitle() const;

        /**
         * get the contents of the article
         */
        string getContent() const;

        /**
         * Setter for next article
         */
        void setNext(Article *);

        /**
         * Getterfor next article
         */
        Article *getNext() const;

        /**
         * Setter for reply article
         */
        void setReply(Article *);

        /**
         * Getter for reply article
         */
        Article *getReply() const;

        /** 
         * returns a string representation of the article
         */
        string toString() const;

        /**
         * Formatted string for syncing servers
         */
        string getSyncString() const;
};

#endif

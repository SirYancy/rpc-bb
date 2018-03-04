#ifndef ARTICLE_H
#define ARTICLE_H

#include <string>

using namespace std;

class Article{
    private: 
        const int id;
        string author;
        string contents;

        Article *next;
        Article *reply;

    public:
        static int articleCount;

        /**
         * Constructor
         */
        Article(const string, const string);

        /**
         * Destructor
         */
        ~Article();
        
        /**
         * get the id of the article
         */
        int getID() const;

        /**
         * get the author of the article
         */
        string getAuthor() const;

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

};

#endif

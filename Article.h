#ifndef ARTICLE_H
#define ARTICLE_H

#include <string>

using namespace std;

class Article{
    private: 
        const int id;
        string author;
        string contents;

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
         * returns a string representation of the article
         */
        string toString() const;
};

#endif

#ifndef POSTINGLIST_H
#define POSTINGLIST_H
#include "SearchRes.h"
#include "MaxMinCountRes.h"

class PostingList {
private:
    class PostingListNode {
    public:
        int lineNum;
        int wordCounter;
        PostingListNode * next;        
    };
    class HeadForPostingListNodes {
    public:
        char* fileName;
        int totalWordCounter;
        int totalLines;
        HeadForPostingListNodes * next;  
        PostingListNode* firstPostingListNode; 
    };
public:
    PostingList();   
    ~PostingList();            
    void insertDataToPostingList(PostingList*, int, char*);  
    void exportDataToSearchRes(SearchRes*,PostingList* );
    void returnMaxCount(PostingList*,MaxMinCountRes*); 
    void returnMinCount(PostingList*,MaxMinCountRes*); 
    HeadForPostingListNodes * firstNode;
    int docCounter;
};

#endif /* POSTINGLIST_H */


#ifndef TRIE_H
#define TRIE_H
#include <stddef.h>
#include "PostingList.h"
#include "Map.h"

class Trie {    
private:
    class Node {
    public:                         
        char character;
        Node * nextListCharacter;   
        Node * nextWordCharacter;
        PostingList * postingListHeadPtr;          
    };    
    void df_visit_delete(Node * n);            
    void df_traverse_delete(Node * n);
public:
    Trie();
    ~Trie();
    void Search(Map*, char*,int);
    void insertWord(char*,int, char*);
    void insertDoc(Map*);
    Node * root;
    PostingList* GetPostingListHeadPtr(char*);
    int totalWords;
};
#endif /* TRIE_H */


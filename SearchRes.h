#ifndef SEARCHRES_H
#define SEARCHRES_H
#include "Map.h"
#include "NetworkFunctions.h"

class SearchRes {    
private:
    class Node {
    public:                         
        char* pathOfFile;
        int lineNum;   
       // char* content;  
        Node * next;
    };  
public:
    Node * headPtr;
    SearchRes();
    ~SearchRes();
    void addNode(char*, int);
    void print();
    char* convertToString(int);
    int numOfRes;
    NetworkFunctions messenger;

     
};

#endif /* SEARCHRES_H */


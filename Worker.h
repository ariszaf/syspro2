#ifndef WORKER_H
#define WORKER_H
#include "Map.h"
#include "Trie.h"
#include "MaxMinCountRes.h"

#include "NetworkFunctions.h"
#include <fstream>

using namespace std;
extern ofstream ofs;
extern  Map ** mapArray;

class Worker {
public:
    Worker();
    Worker(int e2wfd, int w2efd,int workerNum);
    ~Worker();    
    bool importData();
    bool receivePaths();
    void search(char* query);
    void mincount(char * keyword);
    void maxcount(char * keyword);
    void wc();
    void start();
    int countNumOfFiles(char*);
    int importFilesToMaps(char*,int);
    Map** getMapArray() ;
    // ofstream ofs;
    int numOffiles;
    void sendSuccessfulSearches();
private:
    int e2wfd, w2efd;
    std::ofstream file;
    char* logDir;   
    Trie* trie;
    SearchRes* searchres;
    NetworkFunctions messenger;
    int successfulSearches;
};


#endif /* WORKER_H */


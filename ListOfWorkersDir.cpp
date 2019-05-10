#include "ListOfWorkersDir.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>

using namespace std;

ListOfWorkersDir::ListOfWorkersDir() {
    WorkerNodeHeadPtr = NULL;
    numOfPaths = 0;
    lengthOfPaths = 0;
}

ListOfWorkersDir::~ListOfWorkersDir() {
    FilePathNode* temp = this->WorkerNodeHeadPtr;
    while (temp != NULL) {
        FilePathNode* t = temp;
        temp = temp->next;
        delete t;
    }
}

//prosthesi neou path file stin lista me ta path pou nai armodios o worker
void ListOfWorkersDir::assign(char * filePath) {
    if (WorkerNodeHeadPtr == NULL) {
        FilePathNode* node = new FilePathNode();
        node->filePath = filePath;
        numOfPaths++;
        lengthOfPaths += strlen(filePath);
        node->next = NULL;
        WorkerNodeHeadPtr = node;
    } else {
        FilePathNode* node = new FilePathNode();
        node->filePath = filePath;
        numOfPaths++;
        lengthOfPaths += strlen(filePath);
        node->next = WorkerNodeHeadPtr;
        WorkerNodeHeadPtr = node;
    }
}

char * ListOfWorkersDir::merge() {
    char* paths = new char [lengthOfPaths + numOfPaths + 1]();
    FilePathNode* filePathNode = WorkerNodeHeadPtr;
    while (filePathNode) {
        strcat(paths, filePathNode->filePath);
        if (filePathNode->next != NULL) {
            strcat(paths, " ");
        }
        filePathNode = filePathNode->next;
    }
    cout << "Merge " << paths << endl;
    return paths;
}
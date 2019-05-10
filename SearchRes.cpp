#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include <fstream>
#include "SearchRes.h"
#include "ResultParser.h"

using namespace std;

SearchRes::SearchRes() {
    headPtr = NULL;
    numOfRes = 0;
}

SearchRes::~SearchRes() {

    Node* temp = headPtr;
    while (temp != NULL) { // 
        Node* t1 = temp;
        temp = temp->next;
        delete t1;
    }
}

void SearchRes::addNode( char* path, int linenum) {
    if (headPtr == NULL) {
        Node* node = new Node();
        // node->content=content;
        node->lineNum = linenum;
        node->pathOfFile = path;
        numOfRes++;
        node->next = NULL;
        headPtr = node;
    } else {
        Node* tempPtr = headPtr;
        while (tempPtr) {
            if ((strcmp(tempPtr->pathOfFile, path) == 0) && (tempPtr->lineNum == linenum)) {  //petyxenw ton periorismo , na min afinw mia grammi na emfanizetai pollaples fores sta apotelesmata 
                break;
            }
            if (tempPtr->next == NULL) {
                Node* node = new Node();
                // node->content=content;
                node->lineNum = linenum;
                node->pathOfFile = path;
                node->next = NULL;
                tempPtr->next = node;
                numOfRes++;

                break;
            }
            tempPtr = tempPtr->next;
        }
    }
}

void SearchRes::print() {
    Node * tempPtr = headPtr;
    while (tempPtr) {
        cout << "path file" << tempPtr->pathOfFile << endl;
        cout << "line num" << tempPtr->lineNum << endl;
        tempPtr = tempPtr->next;
    }
}

char* SearchRes::convertToString(int numfiles) {
    ResultParser * resParser = new ResultParser(); 

    char **tempArrayOfRes = new char * [numOfRes]();
    char* totalRes;
    int resultLenght = 0, totalResLength = 0;

    Node * tempPtr = headPtr;
    int i = 0;
    while (tempPtr) {
     
        char* searchtostring=resParser->searchToString(tempPtr->pathOfFile, tempPtr->lineNum, numfiles);
        resultLenght = strlen(searchtostring) + 1;
        tempArrayOfRes[i] = new char [resultLenght]();
        strcpy(tempArrayOfRes[i], searchtostring); //perasma kathe keimenou stin antistixi thesi sto pinaka         

        i++;
        delete[] searchtostring;
        tempPtr = tempPtr->next;
        totalResLength += resultLenght;
    }

    char* charNumofRes;
    charNumofRes = (char*) malloc(sizeof numOfRes + 1); //   https://stackoverflow.com/questions/37042323/c-error-when-allocating-dynamic-memory-for-linked-list-node
    sprintf(charNumofRes, "%d", numOfRes);

    totalRes = new char [totalResLength + strlen(charNumofRes) + 1]();
    //sprintf(totalRes, "%s %s", totalRes, charNumofRes);
    strcat(totalRes, charNumofRes);
    strcat(totalRes, " ");
    for (int i = 0; i < numOfRes; i++) {
        strcat(totalRes, tempArrayOfRes[i]);
    }

    free(charNumofRes);
    delete resParser;

    for (int i = 0; i < numOfRes; i++) {
        delete [] tempArrayOfRes[i];
    }
    delete [] tempArrayOfRes;

    return totalRes;
    // messenger.sendMessage(fd, totalRes);
    // cout<<totalRes <<endl;
}

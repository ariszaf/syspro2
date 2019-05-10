#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <math.h>
#include "PostingList.h"
#include "SearchRes.h"
#include "Worker.h"
#include "MaxMinCountRes.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

using namespace std;

PostingList::PostingList() {
    firstNode = NULL;
}

void PostingList::insertDataToPostingList(PostingList* postingListPtr, int linenum, char* filename) {
    if (postingListPtr->firstNode == NULL) { // stin periptwsi pou den yparxei komvos   
        HeadForPostingListNodes* firstHeadForPostingListNodes = new HeadForPostingListNodes();
        firstHeadForPostingListNodes->fileName = filename;
        firstHeadForPostingListNodes->next = NULL;
        firstHeadForPostingListNodes->totalWordCounter = 1;
        firstHeadForPostingListNodes->totalLines = 1;

        PostingListNode* firstPostingListNode = new PostingListNode();
        firstPostingListNode->lineNum = linenum;
        firstPostingListNode->wordCounter = 1;
        firstPostingListNode->next = NULL;

        postingListPtr->docCounter = 1;
        postingListPtr->firstNode = firstHeadForPostingListNodes;
        firstHeadForPostingListNodes->firstPostingListNode = firstPostingListNode;
    } else {
        HeadForPostingListNodes* headTempPtr = postingListPtr->firstNode;
        while (headTempPtr != NULL) { // 
            if (strcmp(filename, headTempPtr->fileName) == 0) { // se periptwsi pou uparxei to arxeio / kai se periptwsi pou uparxei posting list me ayti tin line diaforetika neos komvos
                PostingListNode* plTempPtr = headTempPtr->firstPostingListNode;
                while (plTempPtr != NULL) { // 
                    if (linenum == plTempPtr->lineNum) {
                        plTempPtr->wordCounter++;
                        headTempPtr->totalWordCounter++;
                        return;
                    } else if (plTempPtr->next == NULL) {
                        PostingListNode* postingListNode = new PostingListNode();
                        postingListNode->lineNum = linenum;
                        postingListNode->wordCounter = 1;
                        postingListNode->next = NULL;
                        plTempPtr->next = postingListNode;
                        headTempPtr->totalWordCounter++;
                        headTempPtr->totalLines++;
                        return;
                    }
                    plTempPtr = plTempPtr->next;
                }
            } else if (headTempPtr->next == NULL) { // diaforetika neos komvos afou prokeite gia pliroforia diaforetikou arxeiou      
                HeadForPostingListNodes* firstHeadForPostingListNodes = new HeadForPostingListNodes();
                firstHeadForPostingListNodes->fileName = filename;
                firstHeadForPostingListNodes->next = NULL;
                firstHeadForPostingListNodes->totalWordCounter = 1;
                firstHeadForPostingListNodes->totalLines = 1;

                PostingListNode* firstPostingListNode = new PostingListNode();
                firstPostingListNode->lineNum = linenum;
                firstPostingListNode->wordCounter = 1;
                firstPostingListNode->next = NULL;

                postingListPtr->docCounter++;
                headTempPtr->next = firstHeadForPostingListNodes;
                firstHeadForPostingListNodes->firstPostingListNode = firstPostingListNode;
                break;
            }
            headTempPtr = headTempPtr->next;
        }
    }
}

PostingList::~PostingList() {
    HeadForPostingListNodes* headTempPtr = firstNode;
    while (headTempPtr != NULL) { // 
        PostingListNode* plTempPtr = headTempPtr->firstPostingListNode;
        while (plTempPtr != NULL) { // 
            PostingListNode* t1 = plTempPtr;
            plTempPtr = plTempPtr->next;
            delete t1;
        }

        HeadForPostingListNodes* t2 = headTempPtr;
        headTempPtr = headTempPtr->next;
        delete t2;
    }

}

void PostingList::exportDataToSearchRes(SearchRes* searchres, PostingList* postingListPtr) {
    HeadForPostingListNodes* tempHeadPtr = postingListPtr->firstNode;
    while (tempHeadPtr) {
        PostingListNode* tempPlNode = tempHeadPtr->firstPostingListNode;
        ofs << " : ";
        ofs << tempHeadPtr->fileName;

        while (tempPlNode) {
            searchres->addNode(tempHeadPtr->fileName, tempPlNode->lineNum);
            tempPlNode = tempPlNode->next;
        }
        tempHeadPtr = tempHeadPtr->next;
    }
    ofs << " " << endl;
}

void PostingList::returnMaxCount(PostingList* postingListPtr, MaxMinCountRes* maxCountRes) {

    int tempCounter = 0;
    char* tempFilepath;

    HeadForPostingListNodes* tempHeadPtr = postingListPtr->firstNode;
    while (tempHeadPtr != NULL) {
        if (tempCounter < tempHeadPtr->totalWordCounter) {
            tempCounter = tempHeadPtr->totalWordCounter;
            tempFilepath = tempHeadPtr->fileName;
        } else if (tempCounter == tempHeadPtr->totalWordCounter) {
            if (strcmp(tempHeadPtr->fileName, tempFilepath) < 0) { //to string 1 prohgeitai
                tempCounter = tempHeadPtr->totalWordCounter;
                tempFilepath = tempHeadPtr->fileName;
            }
        }
        tempHeadPtr = tempHeadPtr->next;
    }

    ofs << " : ";
    ofs << tempCounter;
    ofs << " : ";
    ofs << tempFilepath;
    ofs << " " << endl;

    maxCountRes->SetCounter(tempCounter);
    maxCountRes->SetFilePath(tempFilepath);
    // tempFilepath= tempHeadPtr->fileName;
    // delete [] tempFilepath;
}

void PostingList::returnMinCount(PostingList* postingListPtr, MaxMinCountRes* minCountRes) {

    int tempCounter = 0, position = 0;
    char* tempFilepath;

    HeadForPostingListNodes* tempHeadPtr = postingListPtr->firstNode;
    while (tempHeadPtr != NULL) {

        if (position == 0) {
            tempCounter = tempHeadPtr->totalWordCounter;
            tempFilepath = tempHeadPtr->fileName;
        } else if (tempCounter > tempHeadPtr->totalWordCounter) {
            tempCounter = tempHeadPtr->totalWordCounter;
            tempFilepath = tempHeadPtr->fileName;
        } else if (tempCounter == tempHeadPtr->totalWordCounter) {
            if (strcmp(tempHeadPtr->fileName, tempFilepath) < 0) { //to string 1 prohgeitai
                tempCounter = tempHeadPtr->totalWordCounter;
//                tempFilepath = new char [strlen(tempHeadPtr->fileName) + 1];
//                strcpy(tempFilepath, tempHeadPtr->fileName);
                 tempFilepath = tempHeadPtr->fileName;
            }
        }
        tempHeadPtr = tempHeadPtr->next;
        position++;
    }

    ofs << " : ";
    ofs << tempCounter;
    ofs << " : ";
    ofs << tempFilepath;
    ofs << " " << endl;

    minCountRes->SetCounter(tempCounter);
    minCountRes->SetFilePath(tempFilepath);
    // tempFilepath= tempHeadPtr->fileName;
}

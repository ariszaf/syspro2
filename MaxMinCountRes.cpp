#include "MaxMinCountRes.h"
#include "NetworkFunctions.h"
#include "ResultParser.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;
MaxMinCountRes::MaxMinCountRes() {
    counter = 0;
    filePath=NULL;
}

MaxMinCountRes::~MaxMinCountRes() {
    delete [] filePath;
}


int MaxMinCountRes::GetCounter() {
    return counter;
}

void MaxMinCountRes::SetCounter(int counter) {
    this->counter = counter;
}

char* MaxMinCountRes::GetFilePath() {
    return filePath;
}

void MaxMinCountRes::SetFilePath(char* filepath) {

    this->filePath = new char [strlen(filepath) +1];
    strcpy(this->filePath,filepath);
}

char* MaxMinCountRes::convertToString() {           //metatropi se simvoloseira tou antikeimenou me tin voitheia tou antikeimenou resparser, pou to ilopoiiei se xml morfi

    ResultParser * resParser = new ResultParser();

    char* maxminToString = resParser->maxminToString(filePath, counter);
    return maxminToString;
}

void MaxMinCountRes::print() {
    cout << "PATH::: " << filePath << endl;
    cout << "Counter::: " << counter << endl;
}
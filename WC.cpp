#include "WC.h"
#include "ResultParser.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;

WC::WC() {
    totalBytes = 0;
    totalLines = 0;
    totalWords = 0;
}

WC::~WC() { 
    
}

char* WC::convertToString() {

    ResultParser * resParser = new ResultParser();

    char* wctostring = resParser->wcToString(totalBytes, totalLines, totalWords);
 
    delete resParser;

    return wctostring;
}

void WC::setTotalBytes(int counter) {
    this->totalBytes = counter;
}

void WC::setTotalLines(int totalLines) {
    this->totalLines = totalLines;
}

void WC::setTotalWord(int wordCounter) {
    this->totalWords = wordCounter;
}

int WC::getTotalBytes() {
    return totalBytes;
}

int WC::getTotalLines() {
    return totalLines;
}

int WC::getTotalWords() {
    return totalWords;
}

void WC::addByteCounter(int counter) {
    totalBytes += counter;

}

void WC::addLineCounter(int counter) {
    totalLines += counter;

}

void WC::addWordCounter(int counter) {

    totalWords += counter;
}

void WC::print() {
    cout << "Total byte counter " << totalBytes << endl;
    cout << "Total line counter " << totalLines << endl;
    cout << "Total word counter " << totalWords << endl;

}
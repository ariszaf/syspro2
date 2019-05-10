/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CommandParser.h
 * Author: tigris
 *
 * Created on April 15, 2018, 5:12 PM
 */

#ifndef COMMANDPARSER_H
#define COMMANDPARSER_H

#include "NetworkFunctions.h"
#include "SearchRes.h"
#include "WC.h"
#include "MaxMinCountRes.h"

class ResultParser {
public:
    ResultParser();
    char* searchToString( char*,int,int);
    char* maxminToString( char*,int);
    void parseSearch( char*); 
    void parseMaxCounter( char *,MaxMinCountRes*);
    void parseMinCounter( char *,MaxMinCountRes*);
    void parseWc( char*,WC*);
    char* wcToString(int, int, int);
    ~ResultParser();
private:
    NetworkFunctions messenger;
};

#endif /* COMMANDPARSER_H */


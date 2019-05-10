/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MaxMinCountRes.h
 * Author: tigris
 *
 * Created on April 20, 2018, 10:35 PM
 */

#ifndef MAXMINCOUNTRES_H
#define MAXMINCOUNTRES_H
#include "NetworkFunctions.h"

class MaxMinCountRes {
public:
    MaxMinCountRes();
    MaxMinCountRes(MaxMinCountRes& orig);
    ~MaxMinCountRes();
    int GetCounter();
    void SetCounter(int counter);
    char* GetFilePath();
    void SetFilePath(char* filePath);
    char* convertToString();
    void print();

private:
    int counter;
    char* filePath;
    NetworkFunctions messenger;
    
};
#endif /* MAXMINCOUNTRES_H */


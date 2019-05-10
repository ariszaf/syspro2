/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NetworkFunctions.h
 * Author: 
 *
 * Created on April 13, 2018, 1:35 PM
 */

#ifndef NETWORKFUNCTIONS_H
#define NETWORKFUNCTIONS_H

class NetworkFunctions {
public:
    void sendMessage(int fd, char * msg);
    char * receiveMessage(int fd);

};

#endif /* NETWORKFUNCTIONS_H */


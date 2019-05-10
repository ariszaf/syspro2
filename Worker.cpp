#include "Worker.h"
#include "Map.h"
#include "Trie.h"
#include "SearchRes.h"
#include "PostingList.h"
#include "NetworkFunctions.h"
#include "MaxMinCountRes.h"
#include "WC.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <dirent.h>
#include <sys/types.h>
#include <stdio.h> 
using namespace std;

ofstream ofs;
Map ** mapArray;

Worker::Worker(int e2wfd, int w2efd, int workerNum) : e2wfd(e2wfd), w2efd(w2efd) {
    numOffiles = 0;
    successfulSearches=0;
    struct stat st = {0}; //https://stackoverflow.com/questions/7430248/creating-a-new-directory-in-c

    char* charWorkerNum;
    charWorkerNum = (char*) malloc(sizeof workerNum + 1); //  https://stackoverflow.com/questions/37042323/c-error-when-allocating-dynamic-memory-for-linked-list-node
    sprintf(charWorkerNum, "%d", workerNum);

    char charPid[100];
    //charPid = (char*) malloc(sizeof pid +1);              // https://stackoverflow.com/questions/37042323/c-error-when-allocating-dynamic-memory-for-linked-list-node
    sprintf(charPid, "%d", getpid());

    logDir = new char [strlen(" log/worker/ .txt  ") + strlen(charWorkerNum) + strlen(charPid) + 1]();

    if (stat("log", &st) == -1) {
        mkdir("log", 0700);
    }

    strcat(logDir, "log/");
    strcat(logDir, "worker");


    strcat(logDir, charWorkerNum);

    if (stat(logDir, &st) == -1) {
        mkdir(logDir, 0700);
    }

    strcat(logDir, "/");

    //    char pid[1024];
    //    sprintf(pid, "%d", getpid());

    strcat(logDir, charPid);
    strcat(logDir, ".txt");

    ofstream ofs(logDir);
    free(charWorkerNum);
    //  ofs << logDir << endl;
}

Worker::~Worker() {
    close(e2wfd);
    close(w2efd);

    if (ofs.is_open()) {
        ofs.close();
    }

    for (int i = 0; i < numOffiles; i++) {
        delete mapArray[i];
    }
    delete[] mapArray;

    delete[] logDir;
    delete trie;
}

bool Worker::receivePaths() {           //lamvanei ta paths
    char * payload = messenger.receiveMessage(e2wfd);
   // cout << "Worker " << getpid() << " received payload " << payload << endl;
    char* str = new char [strlen(payload) + 1];
    strcpy(str, payload);

    char * pch;
    pch = strtok(str, " \n");

    numOffiles += countNumOfFiles(pch);         //metrisi gia to posa arxeia exei na analavei

    while (pch != NULL) {
        pch = strtok(NULL, " \n");
        if (pch != NULL) {
            numOffiles += countNumOfFiles(pch);
        }
    }

    if (numOffiles == 0) {
        cout << "Wrong directory or empty" << endl;
        delete[] str;
        return false;
    }

    //dimioyrgia enos pinaka apo maps
    mapArray = new Map*[numOffiles]; //!!!! //https://stackoverflow.com/questions/20303820/creation-of-dynamic-array-of-dynamic-objects-in-c
    

    int i = 0;
    char* fil;
    fil = strtok(payload, " \n");
    i += importFilesToMaps(fil, i);

    while (i < numOffiles && fil != NULL) {
        fil = strtok(NULL, " \n");
        if (fil != NULL) {
            i = importFilesToMaps(fil, i);          //dimioyrgia twn maps, ena map gia kathe arxeio
        }
    }
    delete [] payload;
    delete [] str;

    return true;
}

int Worker::importFilesToMaps(char* pch, int i) {
    DIR *dir_ptr; //mema pdf
    struct dirent *direntp;
    if ((dir_ptr = opendir(pch)) == NULL)
        fprintf(stderr, "cannot open %s \n", pch);

    else {
        while ((direntp = readdir(dir_ptr)) != NULL) {
            //printf("%s\n", direntp->d_name);
            if (strcmp(direntp->d_name, ".") != 0 && strcmp(direntp->d_name, "..") != 0) {
                char *fullPath = new char [strlen(direntp->d_name) + strlen(pch) + 2]();
                strcat(fullPath, pch);
                strcat(fullPath, "/");
                strcat(fullPath, direntp->d_name);
                mapArray[i] = new Map(fullPath);
                i++;
                delete [] fullPath;
            }
        }
        closedir(dir_ptr);
    }
    return i;
}

int Worker::countNumOfFiles(char* pch) {        //pigi , simeiwseis Mema
    int counter = 0;
    DIR *dir_ptr; 
    struct dirent *direntp;
    if ((dir_ptr = opendir(pch)) == NULL)
        fprintf(stderr, "cannot open %s \n", pch);

    else {
        while ((direntp = readdir(dir_ptr)) != NULL) {
            if (strcmp(direntp->d_name, ".") != 0 && strcmp(direntp->d_name, "..") != 0) {
                counter++;
            }
        }
        closedir(dir_ptr);
    }
    return counter;
}

bool Worker::importData() {
    bool import = false;
    trie = new Trie();
    for (int i = 0; i < numOffiles; i++) {
        if (mapArray[i]->readFile()) { //diavasma tou arxeiou, prin to perasma twn keimenwn se enan pinaka ginetai elegxos gia lathi sto keimeno 
            trie->insertDoc(mapArray[i]);   //isagogi sto trie olwn twn leksewn apo ola ta maps
            //  mapArray[i]->print();
            import = true;
        }
    }
    return import;
}

void Worker::start() {
    cout << "worker " << getpid() << " etoimos gia iserxomena minimata " << endl;

    while (1) {
        char * payload = messenger.receiveMessage(e2wfd);

        char* str = new char [strlen(payload) + 1];
        strcpy(str, payload);

        char * pch;
        pch = strtok(str, " \n");
        if (!ofs.is_open()) {
            ofs.open(logDir, std::ofstream::out | std::ofstream::app);
        }

        ofs << payload << endl;

        if (strcmp(pch, "/search") == 0) {
            search((char*) payload);
        } else if (strcmp(pch, "/maxcount") == 0) {
            maxcount((char*) payload);
        } else if (strcmp(pch, "/mincount") == 0) {
            mincount((char*) payload);
        } else if (strcmp(pch, "/wc") == 0) {
            wc();
        } else if (strcmp(pch, "/exit") == 0) {
            sendSuccessfulSearches();
            delete [] str;
            delete [] payload;

            break;
        } else {
        }

        delete [] str;
        delete [] payload;

        if (payload == NULL) {
            exit(0);
        }
    }

}

void Worker::search(char* query) {
    struct timeval tmnow; //  https://stackoverflow.com/questions/3673226/how-to-print-time-in-format-2009-08-10-181754-811
    struct tm *tm;
    char buf[30], usec_buf[6];
    bool finder = false;

    searchres = new SearchRes();
    char * pch;
    pch = strtok(query, " "); //http://www.cplusplus.com/forum/beginner/8388/
    while (pch != NULL) {
        pch = strtok(NULL, " ");
        if (pch != NULL) {
            PostingList* tempPl = trie->GetPostingListHeadPtr(pch);


            gettimeofday(&tmnow, NULL);
            tm = localtime(&tmnow.tv_sec);
            strftime(buf, 30, "%Y:%m:%dT%H:%M:%S", tm);
            strcat(buf, ".");
            sprintf(usec_buf, "%d", (int) tmnow.tv_usec);
            strcat(buf, usec_buf);
            if (!ofs.is_open()) {
                ofs.open(logDir, std::ofstream::out | std::ofstream::app);
            }
            //ofs << ""<<endl;
            ofs << buf;                     //katagrafi sto log file ton akrivi xrono
            ofs << " : search : ";
            ofs << pch;                     //tin leksi pros anazitisi
            if (tempPl) {
                tempPl->exportDataToSearchRes(searchres, tempPl);   //an i anazitisi efere apotelesma kapoia posting list , gemizoume to antikeimeno searchres me apotelesmata
                finder = true;
                successfulSearches++;
            } else {
                ofs << " : ";
                ofs << "sorry no result found";
                ofs << " " << endl;
            }
        }
    }
    // searchres->print();
    if (finder) {
        char* result = searchres->convertToString(numOffiles);      //siriopisi me typou opws ginetai me xml twn apotelesmatwn
        messenger.sendMessage(w2efd, result);
        delete [] result;
    } else {
        char errorMess[10];
        strcpy(errorMess, "no-result");
        messenger.sendMessage(w2efd, errorMess);
    }

    delete searchres;
}

void Worker::maxcount(char* keyword) {
    struct timeval tmnow; //  https://stackoverflow.com/questions/3673226/how-to-print-time-in-format-2009-08-10-181754-811
    struct tm *tm;
    char buf[30], usec_buf[6];

    gettimeofday(&tmnow, NULL);
    tm = localtime(&tmnow.tv_sec);
    strftime(buf, 30, "%Y:%m:%dT%H:%M:%S", tm);
    strcat(buf, ".");
    sprintf(usec_buf, "%d", (int) tmnow.tv_usec);
    strcat(buf, usec_buf);

    //first word is operation
    char * k;
    k = strtok(keyword, " \n");
    k = strtok(NULL, " \n");
    if (!ofs.is_open()) {
        ofs.open(logDir, std::ofstream::out | std::ofstream::app);
    }

    ofs << buf;
    ofs << " : maxcount : ";
    ofs << k;

    PostingList* tempPl = trie->GetPostingListHeadPtr(k);
    if (tempPl) {
        MaxMinCountRes* maxMinCountRes = new MaxMinCountRes();      // antikeimeno pou tha krataei oti afora to maxcounter
        tempPl->returnMaxCount(tempPl, maxMinCountRes);             //enimerwsi tou maxMinCountRes 
        // cout << "Top Path" << maxMinCountRes->GetFilePath() << " Counter" << maxMinCountRes->GetCounter() << endl;
        char* convertToString=maxMinCountRes->convertToString();
        messenger.sendMessage(w2efd, convertToString);
        successfulSearches++;
        delete [] convertToString;
        delete maxMinCountRes;
    } else {
        //cout << "i leksi den yparxei" << endl;
        ofs << " : ";
        ofs << "sorry no result found";
        ofs << " " << endl;
        char errorMess[10];
        strcpy(errorMess, "no-result");
        messenger.sendMessage(w2efd, errorMess);
    }
}

void Worker::mincount(char* keyword) {

    struct timeval tmnow; //  https://stackoverflow.com/questions/3673226/how-to-print-time-in-format-2009-08-10-181754-811
    struct tm *tm;
    char buf[30], usec_buf[6];

    gettimeofday(&tmnow, NULL);
    tm = localtime(&tmnow.tv_sec);
    strftime(buf, 30, "%Y:%m:%dT%H:%M:%S", tm);
    strcat(buf, ".");
    sprintf(usec_buf, "%d", (int) tmnow.tv_usec);
    strcat(buf, usec_buf);

    //first word is operation
    char * k;
    k = strtok(keyword, " \n");
    k = strtok(NULL, " \n");

    if (!ofs.is_open()) {
        ofs.open(logDir, std::ofstream::out | std::ofstream::app);
    }

    ofs << buf;
    ofs << " : mincount : ";
    ofs << k;

    PostingList* tempPl = trie->GetPostingListHeadPtr(k);
    if (tempPl) {
        MaxMinCountRes* minCountRes = new MaxMinCountRes();
        tempPl->returnMinCount(tempPl, minCountRes);
        //        cout << "Top Path" << minCountRes->GetFilePath() << " Counter" << minCountRes->GetCounter() << endl;
        messenger.sendMessage(w2efd, minCountRes->convertToString());
        successfulSearches++;
        delete minCountRes;

    } else {
        //cout << "i leksi den yparxei" << endl;
        ofs << " : ";
        ofs << "sorry no result found";
        ofs << " " << endl;
        char errorMess[10];
        strcpy(errorMess, "no-result");
        messenger.sendMessage(w2efd, errorMess);
    }
}

void Worker::sendSuccessfulSearches() {
    char* charCounter;
    charCounter = (char*) malloc(sizeof successfulSearches + 1); //   https://stackoverflow.com/questions/37042323/c-error-when-allocating-dynamic-memory-for-linked-list-node
    sprintf(charCounter, "%d", successfulSearches);

    messenger.sendMessage(w2efd, charCounter);

    free(charCounter);
}


void Worker::wc() {

    struct timeval tmnow; //  https://stackoverflow.com/questions/3673226/how-to-print-time-in-format-2009-08-10-181754-811
    struct tm *tm;
    char buf[30], usec_buf[6];

    gettimeofday(&tmnow, NULL);
    tm = localtime(&tmnow.tv_sec);
    strftime(buf, 30, "%Y:%m:%dT%H:%M:%S", tm);
    strcat(buf, ".");
    sprintf(usec_buf, "%d", (int) tmnow.tv_usec);
    strcat(buf, usec_buf);

    ofs << buf;
    ofs << " : wc : ";

    int byteCounter = 0;
    int lineCounter = 0;
    int wordCounter = trie->totalWords;

    FILE *fp;
    for (int i = 0; i < numOffiles; i++) {          //upologismos twn byte kai lines gia kathe map , dld kathe arxeio
        //   if (mapArray[i]->readFile()) {
        fp = fopen(mapArray[i]->getFileName(), "rb");
        fseek(fp, 0, SEEK_END); //https://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c
        byteCounter += ftell(fp);
        lineCounter += mapArray[i]->getLineCounter();
        fclose(fp);
        //  }
    }

    //  cout << "TOTAL BYTES " << totalBytes << endl;
    ofs << byteCounter;
    ofs << " " << endl;
    WC* wc = new WC();

    wc->setTotalBytes(byteCounter);
    wc->setTotalLines(lineCounter);
    wc->setTotalWord(wordCounter);

    char* wccount = wc->convertToString();

    messenger.sendMessage(w2efd, wccount);

    delete [] wccount;
    delete wc;
}

Map** Worker::getMapArray() {
    return mapArray;
}
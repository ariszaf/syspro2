#include <iostream>
#include "Map.h"
#include "Trie.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>

using namespace std;

Map::Map( char * filename) {
   fileName = new char [strlen(filename) + 1]();
   strcpy(fileName, filename);
}

Map::~Map() {
    if (docArray != NULL) {
        for (int i = 0; i < lineCounter; i++) {
            delete [] docArray[i];
        }
        delete [] docArray;
    }
    delete [] fileName;
}

int Map::countLines() {
    //metrisi twn arithmo twn keimenwn
    FILE * myReadFile = NULL; //https://stackoverflow.com/questions/3501338/c-read-file-line-by-line
    myReadFile = fopen(fileName, "r");
    if (myReadFile != NULL) {
        char * output = NULL;
        size_t len = 0;
        int nread;
        lineCounter = 0;

        while ((nread = getline(&output, &len, myReadFile)) != -1) {
            // cout<<output<<endl;
            // tsekarisma tou keimenou, me paradoxi oti den prepei na einai keno, prepei na exei toulaxiston mia leksi(megethous>0)
//            if (strlen(output) < 3) { //http://www.cplusplus.com/reference/cstring/strlen/
//                cout << "Mikro arxeio keimenoy, ta keimena tou arxeiou prepei na exoun toulaxiston 1 leksi(megethous toulaxiston 1 xaraktira)" << endl;
//                if (output != NULL) {
//                    free(output);
//                }
//                fclose(myReadFile);
//                return 0;
//            }
            lineCounter++;
            free(output);
            output = NULL;
        }
        if (output != NULL) {
            free(output);
        }
        fclose(myReadFile);
    } else {
        cout <<fileName <<" Not found the File or is empty " << endl;
        return 0;
    }
    //cout << "Line counter " << lineCounter << endl; //for debug 
    return lineCounter;
}

bool Map::readFile() {
    if (countLines() == 0) { //metrima twn keimenwn sto arxeio kai elegxos lathwn 
        return false;
    }
    //edw apothikeyw ta lines
    docArray = new char * [lineCounter];  //http://www.cplusplus.com/forum/beginner/26264/

    FILE * myReadFile; //https://stackoverflow.com/questions/3501338/c-read-file-line-by-line
    myReadFile = fopen(fileName, "r");
    if (myReadFile != NULL) {
        char* output = NULL;
        size_t len = 0;
        int nread, currentLine = 0;

        while ((nread = getline(&output, &len, myReadFile)) != -1) { //diavasma tou arxeioy ana grammi
            docArray[currentLine] = new char [len + 1];
            strcpy(docArray[currentLine], output); //perasma kathe keimenou stin antistixi thesi sto pinaka 
            free(output);
            output = NULL;
            currentLine++;
        }
        if (output != NULL) {
            free(output);
        }
        fclose(myReadFile);
    } else {
        cout << "cant open the file" << endl;
        return false;
    }
    return true;
}

void Map::print() {
        cout <<"File "<< fileName << endl;

    cout <<"Total lines "<< lineCounter << endl;
    for (int i = 0; i < lineCounter; i++) {
        cout <<"line "<< i << endl;
        cout << docArray[i] << endl;
    }
}

char** Map::getDocArray() {
    return docArray;
}

int Map::getLineCounter() {
    return lineCounter;
}

 char* Map::getFileName() {
    return fileName;
}
 
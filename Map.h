#ifndef MAP_H
#define MAP_H

class Map {
public:
    Map( char * filename);
    ~Map();
    bool readFile();
    void print();
    char** getDocArray();
    int getLineCounter();
    
    char* getFileName();


private:
    char **docArray; //http://www.cplusplus.com/forum/beginner/26264/
    int lineCounter;
    char* fileName;
    int countLines();
    
};
#endif /* MAP_H */


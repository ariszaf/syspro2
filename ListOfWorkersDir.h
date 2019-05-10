#ifndef MAPOFWORKERS_H
#define MAPOFWORKERS_H

#include <unistd.h>

class ListOfWorkersDir {
private:

    class FilePathNode {
    public:
        char* filePath;
        FilePathNode * next;
    };
public:
    char* execToWorkerName;      
    char* workerToExecutorName;
    int execToWorkerFD;
    int workerToExecutorFD;
    pid_t pid;
    int numOfPaths;
    int lengthOfPaths;
    void assign(char * filePath);
    char* merge();
    ListOfWorkersDir();
    ~ListOfWorkersDir();
    bool hasCompletedCmdCommunication;
        FilePathNode* WorkerNodeHeadPtr;

};



#endif /* MAPOFWORKERS_H */


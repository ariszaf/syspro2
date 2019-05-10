#ifndef JOBEXECUTOR_H
#define JOBEXECUTOR_H
#include "ListOfWorkersDir.h"
#include "Map.h"
#include "Worker.h"
#include "NetworkFunctions.h"

class JobExecutor {
public:
    JobExecutor(char*, int);
    ~JobExecutor();
    void Exit(char*);
    void initialize();
    void createWorkers();
    void sendPathsToWorkers();
    void search(char *, int);
    void mincount(char *);
    void maxcount(char * );
    void wc(char*);
    void search(char*,char*, int);
    void restart(pid_t who);
private:
    int numOfWorkers;
    Map * map;
    ListOfWorkersDir** arrayWithListOfWorkers; 
    NetworkFunctions messenger;
};
#endif /* JOBEXECUTOR_H */


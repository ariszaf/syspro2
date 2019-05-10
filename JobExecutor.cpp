#include "ListOfWorkersDir.h"
#include "JobExecutor.h"
#include "SearchRes.h"
#include "Worker.h"
#include "Map.h"
#include "NetworkFunctions.h"
#include "ResultParser.h"
#include <sys/wait.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/poll.h>
#include "WC.h"
#include "MaxMinCountRes.h"
#include <ctime>

using namespace std;

JobExecutor::JobExecutor(char * filename, int w) {
    signal(SIGPIPE, SIG_IGN); // agnoisi tou simatos sigpipe https://stackoverflow.com/questions/108183/how-to-prevent-sigpipes-or-handle-them-properly 
    map = new Map(filename); // krataei ta paths

    if (!map->readFile()) { //diavasma tou arxeiou, prin to perasma twn keimenwn se enan pinaka ginetai elegxos gia lathi sto keimeno
        delete map; //diagrafi tis domis  kai eksodos se periptwsi lathous
        exit(1);
    }
    numOfWorkers = w;
    arrayWithListOfWorkers = new ListOfWorkersDir * [w]; //pinakas me listes , kathe lista periexei simantikes plirofories gia tous workers   

    for (int i = 0; i < numOfWorkers; i++) {
        arrayWithListOfWorkers[i] = new ListOfWorkersDir();
    }
}

JobExecutor::~JobExecutor() {
    int status;

    for (int i = 0; i < numOfWorkers; i++) {
        close(arrayWithListOfWorkers[i]->execToWorkerFD);
    }

    for (int i = 0; i < numOfWorkers; i++) {
        close(arrayWithListOfWorkers[i]->workerToExecutorFD);
    }
    for (int i = 0; i < numOfWorkers; i++) {
        waitpid(arrayWithListOfWorkers[i]->pid, &status, 0);
    }
    for (int i = 0; i < numOfWorkers; i++) {
        char* buf = arrayWithListOfWorkers[i]->execToWorkerName;
        unlink(buf);

        buf = arrayWithListOfWorkers[i]->workerToExecutorName;
        unlink(buf);
    }

    for (int i = 0; i < numOfWorkers; i++) {
        delete [] arrayWithListOfWorkers[i]->execToWorkerName;
    }


    for (int i = 0; i < numOfWorkers; i++) {
        delete [] arrayWithListOfWorkers[i]->workerToExecutorName;
    }

    for (int i = 0; i < numOfWorkers; i++) {
        delete arrayWithListOfWorkers[i];
    }


    delete [] arrayWithListOfWorkers;
    delete map;
}

void JobExecutor::initialize() {
    int x;
    for (int i = 0; i < numOfWorkers; i++) {
        //dimioyrgia fifos
        //https://www.geeksforgeeks.org/named-pipe-fifo-example-c-program
        //https://stackoverflow.com/questions/2784500/how-to-send-a-simple-string-between-two-programs-using-pipes
        arrayWithListOfWorkers[i]->execToWorkerName = new char[strlen("exec_to_worker") + 10];
        arrayWithListOfWorkers[i]->workerToExecutorName = new char[strlen("worker_to_exec") + 10];

        sprintf(arrayWithListOfWorkers[i]->execToWorkerName, "exec_to_worker_%d.fifo", i);
        sprintf(arrayWithListOfWorkers[i]->workerToExecutorName, "worker_to_exec_%d.fifo", i);

        char * buf = arrayWithListOfWorkers[i]->execToWorkerName;

        x = mkfifo(buf, 0666);
        if (x == -1 && errno != EEXIST) {
            perror("mkfifo failed");
            exit(-1);
        }
        if (x == -1 && errno == EEXIST) {
            unlink(buf);
            x = mkfifo(buf, 0666);
        }

        buf = arrayWithListOfWorkers[i]->workerToExecutorName;
        x = mkfifo(buf, 0666);

        if (x == -1 && errno != EEXIST) {
            perror("mkfifo failed");
            exit(-1);
        }
        if (x == -1 && errno == EEXIST) {
            unlink(buf);
            x = mkfifo(buf, 0666);
        }
    }

    for (int i = 0; i < map->getLineCounter(); i++) { //anathesi stin domi   arrayWithListOfWorkers ,tou job executor ta monopatia tou kathe worker, omoiomorfi anathesi    
        arrayWithListOfWorkers[i % numOfWorkers]->assign(map->getDocArray()[i]);
    }
}

void JobExecutor::sendPathsToWorkers() {
    NetworkFunctions messenger;

    for (int i = 0; i < numOfWorkers; i++) {
        char * payload = arrayWithListOfWorkers[i]->merge();
        messenger.sendMessage(arrayWithListOfWorkers[i]->execToWorkerFD, payload);
        delete [] payload;
    }
}

void JobExecutor::createWorkers() { //dimiourgia twn worker 
    for (int i = 0; i < numOfWorkers; i++) {
        pid_t p = fork();
        if (p == -1) {
            perror("fork failed");
            exit(1);
        } else if (p > 0) {
            arrayWithListOfWorkers[i]->pid = p;
        } else if (p == 0) { // p == 0 (child process)
            cout << "Worker " << getpid() << endl;

            int e2wfd = open(arrayWithListOfWorkers[i]->execToWorkerName, O_RDONLY);
            int w2efd = open(arrayWithListOfWorkers[i]->workerToExecutorName, O_WRONLY);

            Worker* worker = new Worker(e2wfd, w2efd, i);
            if (!worker->receivePaths()) {
                delete worker;
                exit(1);
            }
            if (!worker->importData()) {
                delete worker;
                exit(1);
            }
            worker->start();
            delete worker;
            exit(1);
        }
    }

    cout << "O Executor perimenei tous workers na suxronistoun " << endl;
    for (int i = 0; i < numOfWorkers; i++) {
        arrayWithListOfWorkers[i]->execToWorkerFD = open(arrayWithListOfWorkers[i]->execToWorkerName, O_WRONLY);
        cout << "Worker " << i << " synched with " << arrayWithListOfWorkers[i]->execToWorkerName << " pipe " << endl;
    }
    for (int i = 0; i < numOfWorkers; i++) {
        arrayWithListOfWorkers[i]->workerToExecutorFD = open(arrayWithListOfWorkers[i]->workerToExecutorName, O_RDONLY);
        cout << "Worker " << i << " synched with " << arrayWithListOfWorkers[i]->workerToExecutorName << " << pipe " << endl;
    }
    cout << "O Executor exei suxronistei me olous tous workers " << endl;
}

void JobExecutor::search(char* operation, char* command, int d) { //https://stackoverflow.com/questions/3220477/how-to-use-clock-in-c
    //piges gia poll
    //http://man7.org/linux/man-pages/man2/poll.2.html
    //http://www.unixguide.net/unix/programming/2.1.2.shtml

    bool result = false;
    int completedWorkers = 0;
    int hasSent = 0;
    clock_t start;
    double duration;

    for (int i = 0; i < numOfWorkers; i++) {
        arrayWithListOfWorkers[i]->hasCompletedCmdCommunication = false;
    }

    start = clock();

    for (int i = 0; i < numOfWorkers; i++) {
        messenger.sendMessage(arrayWithListOfWorkers[i]->execToWorkerFD, command);
        //cout << "search query sent to workers " << endl;
    }

    while (completedWorkers < numOfWorkers) {
        //  cout << "Executor enters poll loop " << completedWorkers << " of " << numOfWorkers << " completed " << endl;

        // ipologismos posoi workers den exoun apantisei
        int missingWorkers = 0;
        for (int i = 0; i < numOfWorkers; i++) {
            if (!arrayWithListOfWorkers[i]->hasCompletedCmdCommunication) {
                missingWorkers++;
            }
        }

        // dimiourgia pinaka me tous ekremeis descriptors
        struct pollfd * array = new struct pollfd[missingWorkers];

        for (int i = 0, j = 0; i < numOfWorkers; i++) {
            if (!arrayWithListOfWorkers[i]->hasCompletedCmdCommunication) {
                array[j].events = POLLIN | POLLHUP;
                array[j].revents = 0;
                array[j++].fd = arrayWithListOfWorkers[i]->workerToExecutorFD;
            }
        }


        // i poll perimenei mexri na erthoun dedomena i mexri enas worker kleisei to pipe poll
     //   cout << "O executor perimenei apo tous workers na steiloun dedomena ..." << endl;
        int retval = poll(array, missingWorkers, -1);

        if (retval == -1) {
            perror("poll critical error");
            exit(0);
        }

        //  an exei sumvei kati scan to poll array gia POLLI or POLLHUP
        if (retval > 0) {
            for (int j = 0; j < missingWorkers; j++) {
                if ((array[j].revents & POLLIN) != 0) {
                    duration = (clock() - start) / (double) CLOCKS_PER_SEC;
                    char* payload = messenger.receiveMessage(array[j].fd);

                    if (duration <= d) { //metraw mono osa apotelesmata irthan mesa sto sugkekrimeno deadline   
                        if (strcmp(payload, "no-result") != 0) {
                            ResultParser* resParser = new ResultParser();
                            resParser->parseSearch(payload);
                            result = true;
                            hasSent++;
                            delete resParser;
                        } else {
                            //  cout << command << " :No result" << endl;
                        }
                    } else {
                        //cout<<"deadline error"<<endl;
                    }
                    delete [] payload;

                    // auksisi received_payloads gia kathe worker pou eksipiretithike
                    for (int i = 0; i < numOfWorkers; i++) {
                        if (arrayWithListOfWorkers[i]->workerToExecutorFD == array[j].fd) {
                            if (arrayWithListOfWorkers[i]->hasCompletedCmdCommunication == false) {
                                arrayWithListOfWorkers[i]->hasCompletedCmdCommunication = true;
                                completedWorkers++;
                            }
                            break;
                        }
                    }
                }
                if ((array[j].revents & POLLHUP) != 0) {
                    // auksisi received_payloads gia kathe worker pou eksipiretithike
                    for (int i = 0; i < numOfWorkers; i++) {
                        if (arrayWithListOfWorkers[i]->workerToExecutorFD == array[j].fd) {
                            if (arrayWithListOfWorkers[i]->hasCompletedCmdCommunication == false) {
                                arrayWithListOfWorkers[i]->hasCompletedCmdCommunication = true;
                                completedWorkers++;
                            }
                            break;
                        }
                    }
                }
            }
        }
        delete [] array;
    }

    cout << hasSent << " worker apantisan apo tous " << numOfWorkers << endl;
    if (!result) {
        cout << command << " :No result" << endl;
    }
}

void JobExecutor::wc(char* line) {
    int completedWorkers = 0;
    WC* totalWc = new WC();

    for (int i = 0; i < numOfWorkers; i++) { //arxikopoiisi boolean gia to an mas exei steilei data
        arrayWithListOfWorkers[i]->hasCompletedCmdCommunication = false;
    }

    // ----------------------------------------------------
    for (int i = 0; i < numOfWorkers; i++) {
        messenger.sendMessage(arrayWithListOfWorkers[i]->execToWorkerFD, line);
    }
    // ----------------------------------------------------

    while (completedWorkers < numOfWorkers) {
        //    cout << "Executor enters poll loop " << completedWorkers << " of " << numOfWorkers << " completed " << endl;
        // ipologismos posoi workers den exoun apantisei
        int missingWorkers = 0;

        for (int i = 0; i < numOfWorkers; i++) {
            if (!arrayWithListOfWorkers[i]->hasCompletedCmdCommunication) {
                missingWorkers++;
            }
        }

        // dimiourgia pinaka me tous ekremeis descriptors
        struct pollfd * array = new struct pollfd[missingWorkers];

        for (int i = 0, j = 0; i < numOfWorkers; i++) {
            if (!arrayWithListOfWorkers[i]->hasCompletedCmdCommunication) {
                array[j].events = POLLIN | POLLHUP;
                array[j].revents = 0;
                array[j++].fd = arrayWithListOfWorkers[i]->workerToExecutorFD;
            }
        }

        // i poll perimenei mexri na erthoun dedomena i mexri enas worker kleisei to pipe poll
      //  cout << "O executor perimenei apo tous workers na steiloun dedomena ..." << endl;
        int retval = poll(array, missingWorkers, -1);

        if (retval == -1) {
            perror("poll critical error");
            exit(0);
        }

        //  an exei sumvei kati scan to poll array gia POLLI or POLLHUP
        if (retval > 0) {
            for (int j = 0; j < missingWorkers; j++) {
                if ((array[j].revents & POLLIN) != 0) {

                    ResultParser* resParser = new ResultParser();
                    char* payload = messenger.receiveMessage(array[j].fd);

                    resParser->parseWc(payload, totalWc);

                    delete resParser;
                    delete [] payload;

                    // auksisi received_payloads gia kathe worker pou eksipiretithike
                    for (int i = 0; i < numOfWorkers; i++) {
                        if (arrayWithListOfWorkers[i]->workerToExecutorFD == array[j].fd) {
                            if (arrayWithListOfWorkers[i]->hasCompletedCmdCommunication == false) {
                                arrayWithListOfWorkers[i]->hasCompletedCmdCommunication = true;
                                completedWorkers++;
                            }
                            break;
                        }
                    }
                }
                if ((array[j].revents & POLLHUP) != 0) {
                    // auksisi received_payloads gia kathe worker pou eksipiretithike
                    for (int i = 0; i < numOfWorkers; i++) {
                        if (arrayWithListOfWorkers[i]->workerToExecutorFD == array[j].fd) {
                            if (arrayWithListOfWorkers[i]->hasCompletedCmdCommunication == false) {
                                arrayWithListOfWorkers[i]->hasCompletedCmdCommunication = true;
                                completedWorkers++;
                            }
                            break;
                        }
                    }
                }
            }
        }

        delete [] array;
    }
    totalWc->print();
    delete totalWc;
}

void JobExecutor::maxcount(char* keyword) {
    bool result = false;

    MaxMinCountRes* maxcountRes = new MaxMinCountRes();
    int completedWorkers = 0;
    for (int i = 0; i < numOfWorkers; i++) {
        arrayWithListOfWorkers[i]->hasCompletedCmdCommunication = false;
    }

    for (int i = 0; i < numOfWorkers; i++) {
        messenger.sendMessage(arrayWithListOfWorkers[i]->execToWorkerFD, keyword);
    }

    while (completedWorkers < numOfWorkers) {
        //  cout << "Executor enters poll loop " << completedWorkers << " of " << numOfWorkers << " completed " << endl;
        
        // ipologismos posoi workers den exoun apantisei
        int missingWorkers = 0;

        for (int i = 0; i < numOfWorkers; i++) {
            if (!arrayWithListOfWorkers[i]->hasCompletedCmdCommunication) {
                missingWorkers++;
            }
        }

        // dimiourgia pinaka me tous ekremeis descriptors
        struct pollfd * array = new struct pollfd[missingWorkers];

        for (int i = 0, j = 0; i < numOfWorkers; i++) {
            if (!arrayWithListOfWorkers[i]->hasCompletedCmdCommunication) {
                array[j].events = POLLIN | POLLHUP;
                array[j].revents = 0;
                array[j++].fd = arrayWithListOfWorkers[i]->workerToExecutorFD;
            }
        }

        // i poll perimenei mexri na erthoun dedomena i mexri enas worker kleisei to pipe poll
       // cout << "O executor perimenei apo tous workers na steiloun dedomena ..." << endl;
        int retval = poll(array, missingWorkers, -1);

        if (retval == -1) {
            perror("poll critical error");
            exit(0);
        }

        //  an exei sumvei kati scan to poll array gia POLLI or POLLHUP
        if (retval > 0) {
            for (int j = 0; j < missingWorkers; j++) {
                if ((array[j].revents & POLLIN) != 0) {


                    char* payload = messenger.receiveMessage(array[j].fd);

                    if (strcmp(payload, "no-result") != 0) {
                        ResultParser* resParser = new ResultParser();
                        resParser->parseMaxCounter(payload, maxcountRes);
                        result = true;
                        delete resParser;
                    } else {
                        //cout << keyword << " :No result" << endl;
                    }

                    delete [] payload;

                    // auksisi received_payloads gia kathe worker pou eksipiretithike
                    for (int i = 0; i < numOfWorkers; i++) {
                        if (arrayWithListOfWorkers[i]->workerToExecutorFD == array[j].fd) {
                            if (arrayWithListOfWorkers[i]->hasCompletedCmdCommunication == false) {
                                arrayWithListOfWorkers[i]->hasCompletedCmdCommunication = true;
                                completedWorkers++;
                            }
                            break;
                        }
                    }
                }
                if ((array[j].revents & POLLHUP) != 0) {
                    // auksisi received_payloads gia kathe worker pou eksipiretithike
                    for (int i = 0; i < numOfWorkers; i++) {
                        if (arrayWithListOfWorkers[i]->workerToExecutorFD == array[j].fd) {
                            if (arrayWithListOfWorkers[i]->hasCompletedCmdCommunication == false) {
                                arrayWithListOfWorkers[i]->hasCompletedCmdCommunication = true;
                                completedWorkers++;
                            }
                            break;
                        }
                    }
                }
            }
        }
        
        delete [] array;
    }
    if (result) {
        maxcountRes->print();
    } else {
        cout << keyword << " :No result" << endl;

    }
    delete maxcountRes;
}

void JobExecutor::mincount(char* keyword) {

    bool result = false;

    MaxMinCountRes* mincountRes = new MaxMinCountRes();

    int completedWorkers = 0;
    for (int i = 0; i < numOfWorkers; i++) {
        arrayWithListOfWorkers[i]->hasCompletedCmdCommunication = false;
    }

    for (int i = 0; i < numOfWorkers; i++) {
        messenger.sendMessage(arrayWithListOfWorkers[i]->execToWorkerFD, keyword);
    }


    while (completedWorkers < numOfWorkers) {
        // cout << "Executor enters poll loop " << completedWorkers << " of " << numOfWorkers << " completed " << endl;
        // ipologismos posoi workers den exoun apantisei
        int missingWorkers = 0;

        for (int i = 0; i < numOfWorkers; i++) {
            if (!arrayWithListOfWorkers[i]->hasCompletedCmdCommunication) {
                missingWorkers++;
            }
        }

        // dimiourgia pinaka me tous ekremeis descriptors

        struct pollfd * array = new struct pollfd[missingWorkers];

        for (int i = 0, j = 0; i < numOfWorkers; i++) {
            if (!arrayWithListOfWorkers[i]->hasCompletedCmdCommunication) {
                array[j].events = POLLIN | POLLHUP;
                array[j].revents = 0;
                array[j++].fd = arrayWithListOfWorkers[i]->workerToExecutorFD;
            }
        }

        // i poll perimenei mexri na erthoun dedomena i mexri enas worker kleisei to pipe poll
      // cout << "O executor perimenei apo tous workers na steiloun dedomena ..." << endl;
        int retval = poll(array, missingWorkers, -1);

        if (retval == -1) {
            perror("poll critical error");
            exit(0);
        }

        //  an exei sumvei kati scan to poll array gia POLLI or POLLHUP
        if (retval > 0) {
            for (int j = 0; j < missingWorkers; j++) {
                if ((array[j].revents & POLLIN) != 0) {


                    char* payload = messenger.receiveMessage(array[j].fd);

                    if (strcmp(payload, "no-result") != 0) {
                        ResultParser* resParser = new ResultParser();
                        resParser->parseMinCounter(payload, mincountRes);
                        result = true;
                        delete resParser;
                    } else {
                        // cout << keyword << " :No result" << endl;
                    }

                    delete [] payload;

                    // auksisi received_payloads gia kathe worker pou eksipiretithike
                    for (int i = 0; i < numOfWorkers; i++) {
                        if (arrayWithListOfWorkers[i]->workerToExecutorFD == array[j].fd) {
                            if (arrayWithListOfWorkers[i]->hasCompletedCmdCommunication == false) {
                                arrayWithListOfWorkers[i]->hasCompletedCmdCommunication = true;
                                completedWorkers++;
                            }
                            break;
                        }
                    }
                }
                if ((array[j].revents & POLLHUP) != 0) {
                    // auksisi received_payloads gia kathe worker pou eksipiretithike
                    for (int i = 0; i < numOfWorkers; i++) {
                        if (arrayWithListOfWorkers[i]->workerToExecutorFD == array[j].fd) {
                            if (arrayWithListOfWorkers[i]->hasCompletedCmdCommunication == false) {
                                arrayWithListOfWorkers[i]->hasCompletedCmdCommunication = true;
                                completedWorkers++;
                            }
                            break;
                        }
                    }
                }
            }
        }
        delete [] array;
    }
    if (result) {
        mincountRes->print();
    } else {
        cout << keyword << " :No result" << endl;

    }
    delete mincountRes;
}

void JobExecutor::Exit(char* str) {

    int completedWorkers = 0;
    int totalSuccessfulSearches = 0;
    for (int i = 0; i < numOfWorkers; i++) {
        arrayWithListOfWorkers[i]->hasCompletedCmdCommunication = false;
    }

    // ----------------------------------------------------
    for (int i = 0; i < numOfWorkers; i++) {
        messenger.sendMessage(arrayWithListOfWorkers[i]->execToWorkerFD, str);
    }
    // ----------------------------------------------------

    while (completedWorkers < numOfWorkers) {
        // cout << "Executor enters poll loop " << completedWorkers << " of " << numOfWorkers << " completed " << endl;
        // count how many workers have not responded

        int missingWorkers = 0;

        for (int i = 0; i < numOfWorkers; i++) {
            if (!arrayWithListOfWorkers[i]->hasCompletedCmdCommunication) {
                missingWorkers++;
            }
        }

        // dimiourgia pinaka me tous ekremeis descriptors

        struct pollfd * array = new struct pollfd[missingWorkers];

        for (int i = 0, j = 0; i < numOfWorkers; i++) {
            if (!arrayWithListOfWorkers[i]->hasCompletedCmdCommunication) {
                array[j].events = POLLIN | POLLHUP;
                array[j].revents = 0;
                array[j++].fd = arrayWithListOfWorkers[i]->workerToExecutorFD;
            }
        }

        // i poll perimenei mexri na erthoun dedomena i mexri enas worker kleisei to pipe poll
      //  cout << "O executor perimenei apo tous workers na steiloun dedomena ..." << endl;
        int retval = poll(array, missingWorkers, -1);

        if (retval == -1) {
            perror("poll critical error");
            exit(0);
        }

        //  an exei sumvei kati scan to poll array gia POLLI or POLLHUP
        if (retval > 0) {
            for (int j = 0; j < missingWorkers; j++) {
                if ((array[j].revents & POLLIN) != 0) {

                    char* payload = messenger.receiveMessage(array[j].fd);
                    totalSuccessfulSearches += atoi(payload);
                    delete [] payload;

                    // auksisi received_payloads gia kathe worker pou eksipiretithike
                    for (int i = 0; i < numOfWorkers; i++) {
                        if (arrayWithListOfWorkers[i]->workerToExecutorFD == array[j].fd) {
                            if (arrayWithListOfWorkers[i]->hasCompletedCmdCommunication == false) {
                                arrayWithListOfWorkers[i]->hasCompletedCmdCommunication = true;
                                completedWorkers++;
                            }
                            break;
                        }
                    }
                }
                if ((array[j].revents & POLLHUP) != 0) {
                    // auksisi received_payloads gia kathe worker pou eksipiretithike
                    for (int i = 0; i < numOfWorkers; i++) {
                        if (arrayWithListOfWorkers[i]->workerToExecutorFD == array[j].fd) {
                            if (arrayWithListOfWorkers[i]->hasCompletedCmdCommunication == false) {
                                arrayWithListOfWorkers[i]->hasCompletedCmdCommunication = true;
                                completedWorkers++;
                            }
                            break;
                        }
                    }
                }
            }
        }

        
        delete [] array;
    }
    cout << "Total Successful Searches " << totalSuccessfulSearches << endl;
}

//restart tou xamenou worker kai tou dinoume oti eixe na analavei apo prin
void JobExecutor::restart(pid_t terminated_pid) {
    cout << "restarting worker in place of pid: " << terminated_pid << endl;

    for (int i = 0; i < numOfWorkers; i++) {
        if (arrayWithListOfWorkers[i]->pid == terminated_pid) {

            close(arrayWithListOfWorkers[i]->execToWorkerFD);
            close(arrayWithListOfWorkers[i]->workerToExecutorFD);

            unlink(arrayWithListOfWorkers[i]->execToWorkerName);
            unlink(arrayWithListOfWorkers[i]->workerToExecutorName);

            mkfifo(arrayWithListOfWorkers[i]->execToWorkerName, 0666);
            mkfifo(arrayWithListOfWorkers[i]->workerToExecutorName, 0666);

            arrayWithListOfWorkers[i]->pid = fork();

            if (arrayWithListOfWorkers[i]->pid == 0) {
                cout << "Hello from worker " << getpid() << endl;

                int e2wfd = open(arrayWithListOfWorkers[i]->execToWorkerName, O_RDONLY);
                int w2efd = open(arrayWithListOfWorkers[i]->workerToExecutorName, O_WRONLY);

                Worker* worker = new Worker(e2wfd, w2efd, i);
                if (!worker->receivePaths()) {
                    delete worker;
                    exit(1);
                }
                if (!worker->importData()) {
                    delete worker;
                    exit(1);
                }
                worker->start();
                delete worker;
                exit(1);
            } else {
                cout << "Executor is waiting for new to synch " << endl;

                arrayWithListOfWorkers[i]->execToWorkerFD = open(arrayWithListOfWorkers[i]->execToWorkerName, O_WRONLY);
                arrayWithListOfWorkers[i]->workerToExecutorFD = open(arrayWithListOfWorkers[i]->workerToExecutorName, O_RDONLY);
                cout << "Executor synchronized with new workers " << endl;
                char * payload = arrayWithListOfWorkers[i]->merge();
                messenger.sendMessage(arrayWithListOfWorkers[i]->execToWorkerFD, payload);
                delete [] payload;
            }
        }
    }
}
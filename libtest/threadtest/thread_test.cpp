
#include "gipcy.h"

#include <iostream>
#include <string>
#include <istream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <signal.h>
#include <vector>

using namespace std;

//------------------------------------------------------------------

unsigned int exit_flag = 0;

void sighandler(int sig)
{
    fprintf(stderr, "SIGNAL: %d\n", sig);
    exit_flag = 1;
}

//------------------------------------------------------------------

thread_value thread_function(void *param)
{
    unsigned int* exit_flag = (unsigned int*)param;
    fprintf(stderr, "start thread: 0x%x\n", IPC_threadID());
    while(!exit_flag[0]) {
      IPC_delay(20);
    }
    fprintf(stderr, "stop thread: 0x%x\n", IPC_threadID());
    return (thread_value)IPC_threadID();
}

//------------------------------------------------------------------

int main(int argc, char* argv[])
{
    std::cout << "TEST STARTED" << endl;

    signal(SIGINT, sighandler);

    if(IPC_init() < 0) {
        std::cout << "IPC_init() - Erorr" << endl;
        return -1;
    }

    std::vector<IPC_handle> threads;

    for(unsigned i=0; i<3; i++) {
      IPC_handle h = IPC_createThread(0, thread_function, &exit_flag);
      if(!h) {
        fprintf(stderr, "Can'create thread: %d\n", i);
        break;
      }
      threads.push_back(h);
    }

    while(!exit_flag);

    IPC_delay(1000);

    for(unsigned i=0; i<threads.size(); i++) {
      IPC_handle h = threads.at(i);
      IPC_waitThread(h, -1);
      IPC_deleteThread(h);
    }

    threads.clear();

    IPC_cleanup();

    std::cout << "TEST COMPLETE" << endl;

    return 0;
}

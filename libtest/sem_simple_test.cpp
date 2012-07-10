
#include "gipcy.h"

#include <iostream>
#include <string>
#include <istream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <signal.h>

using namespace std;

//------------------------------------------------------------------

void show_error(int err)
{
    switch(err) {
    case IPC_OK:
        std::cout << "IPC_OK" << endl;
    break;
    case IPC_WAIT_TIMEOUT:
        std::cout << "IPC_WAIT_TIMEOUT" << endl;
    break;
    case IPC_GENERAL_ERROR:
        std::cout << "IPC_GENERAL_ERROR" << endl;
    break;
    }
}

//------------------------------------------------------------------

int main(int argc, char* argv[])
{
    std::cout << "Semaphore test start" << endl;
    
    std::cout << "Create synchronize semaphore..." << endl;
    IPC_handle lockSem = IPC_createSemaphore("lockSem", 1);
    if(!lockSem) {
        std::cout << "IPC_createSemaphore() failed" << endl;
        return -1;
    }

    std::cout << "Ok" << endl;

    int res = IPC_lockSemaphore(lockSem, -1);
    if(res != IPC_OK) {
        std::cout << "IPC_lockSemaphore() failed" << endl;
        show_error(res);
    }
    
    std::cout << "Semaphore locked..." << endl;
    getchar();

    res = IPC_unlockSemaphore(lockSem);
    if(res != IPC_OK) {
        std::cout << "IPC_unlockSemaphore() failed" << endl;
        show_error(res);
    }
    
    std::cout << "Semaphore unlocked..." << endl;
    getchar();

    std::cout << "Delete semaphore..." << endl;
    IPC_deleteSemaphore(lockSem);

    std::cout << "Ok" << endl;

    return 0;
}

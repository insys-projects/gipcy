
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
    if(IPC_init() < 0) {
        std::cout << "IPC_init() - Erorr" << endl;
        return -1;
    }

    IPC_handle lockSem = IPC_createSemaphore("lockSem", 1);
    if(!lockSem) {
        std::cout << "IPC_createSemaphore() failed" << endl;
        return -1;
    } else {

    std::cout << "IPC_createSemaphore(1) - Ok" << endl;
}

    int res = IPC_lockSemaphore(lockSem, -1);
    if(res != IPC_OK) {
        std::cout << "IPC_lockSemaphore() - Error" << endl;
    } else {

    std::cout << "IPC_lockSemaphore(-1) - Ok" << endl;
 }

    res = IPC_lockSemaphore(lockSem, 1000);
    if(res != IPC_OK) {
        std::cout << "IPC_lockSemaphore(1000) - Error" << endl;
    }

    res = IPC_unlockSemaphore(lockSem);
    if(res != IPC_OK) {
        std::cout << "IPC_unlockSemaphore() - Error" << endl;
    } else {

    std::cout << "IPC_unlockSemaphore() - Ok" << endl;
}

    res = IPC_deleteSemaphore(lockSem);
    if(res != IPC_OK) {
        std::cout << "IPC_deleteSemaphore() - Error" << endl;
    } else {

    std::cout << "IPC_deleteSemaphore() - Ok" << endl;
}

    IPC_cleanup();

    std::cout << "TEST COMPLETE" << endl;

    return 0;
}

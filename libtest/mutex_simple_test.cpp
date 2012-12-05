
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
    std::cout << "Mutex test start" << endl;
    IPC_init();
    std::cout << "Create synchronize mutex..." << endl;
    IPC_handle lockMutex = IPC_createMutex("lockMutex", 0);
    if(!lockMutex) {
        std::cout << "IPC_createMutex() failed" << endl;
        return -1;
    }

    std::cout << "Ok" << endl;

    int res = IPC_captureMutex(lockMutex, -1);
    if(res != IPC_OK) {
        std::cout << "IPC_captureMutex() failed" << endl;
        show_error(res);
    }
    
    std::cout << "Mutex locked..." << endl;
    getchar();

    res = IPC_releaseMutex(lockMutex);
    if(res != IPC_OK) {
        std::cout << "IPC_releaseMutex() failed" << endl;
        show_error(res);
    }
    
    std::cout << "Mutex unlocked..." << endl;
    getchar();

    std::cout << "Delete mutex..." << endl;
    IPC_deleteMutex(lockMutex);
    IPC_cleanup();
    std::cout << "Ok" << endl;

    return 0;
}

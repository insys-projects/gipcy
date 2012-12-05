
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

    IPC_handle shm = IPC_createSharedMemory("testShm", 0x1000);
    if(!shm) {
        std::cout << "IPC_createSharedMemory() - Error" << endl;
        return -1;
    } else {
        std::cout << "IPC_createSharedMemory() - Ok" << endl;
    }
    
    int isExist = 0;
    IPC_handle shmExist = IPC_createSharedMemoryEx("testShm", 0x1000, &isExist);
    if(!shmExist) {
        std::cout << "IPC_createSharedMemoryEx() - Error" << endl;
        return -1;
    } else {
        std::cout << "IPC_createSharedMemoryEx() - Ok. isExist = " << isExist << endl;
    }
    
    getchar();
    
    int res = IPC_deleteSharedMemory(shmExist);
    if(res != IPC_OK) {
        std::cout << "IPC_deleteSharedMemory() - Error" << endl;
    } else {
        std::cout << "IPC_deleteSharedMemory() - Ok" << endl;
    }

    void *addr = IPC_mapSharedMemory(shm);
    if(!addr) {
        std::cout << "IPC_mapSharedMemory() - Error" << endl;
    } else {
       std::cout << "IPC_mapSharedMemory() - Ok" << endl;
    }

    std::cout << "Press Eneter to continue..." << endl;
    getchar();

    res = IPC_unmapSharedMemory(shm);
    if(res != IPC_OK) {
        std::cout << "IPC_unmapSharedMemory() - Error" << endl;
    } else {
        std::cout << "IPC_unmapSharedMemory() - Ok" << endl;
    }

    res = IPC_deleteSharedMemory(shm);
    if(res != IPC_OK) {
        std::cout << "IPC_deleteSharedMemory() - Error" << endl;
    } else {
        std::cout << "IPC_deleteSharedMemory() - Ok" << endl;
    }

    IPC_cleanup();

    std::cout << "TEST COMPLETE" << endl;

    return 0;
}

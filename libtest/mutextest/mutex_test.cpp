
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

int main(int argc, char* argv[])
{
    if(IPC_init() < 0) {
        std::cout << "IPC_init() - Error" << endl;
        return -1;
    }

    IPC_handle lockMutex = IPC_createMutex("lockMutex", 0);
    if(!lockMutex) {
        std::cout << "IPC_createMutex() - Error" << endl;
        return -1;
    } else {

    std::cout << "IPC_createMutex(0) - Ok" << endl;
}

    int res = IPC_captureMutex(lockMutex, 1000);
    if(res != IPC_OK) {
        std::cout << "IPC_captuteMutex(1000) - Error" << endl;
    } else {

    std::cout << "IPC_captureMutex(1000) - Ok" << endl;
 }

    res = IPC_releaseMutex(lockMutex);
    if(res != IPC_OK) {
        std::cout << "IPC_releaseMutex() - Error" << endl;
    } else {

    std::cout << "IPC_releaseMutex() - Ok" << endl;
}

    res = IPC_captureMutex(lockMutex, -1);
    if(res != IPC_OK) {
        std::cout << "IPC_captureMutex(-1) - Error" << endl;
    } else {
    std::cout << "IPC_captureMutex(-1) - Ok" << endl;
}

    res = IPC_releaseMutex(lockMutex);
    if(res != IPC_OK) {
        std::cout << "IPC_releaseMutex() - Error" << endl;
    } else {

    std::cout << "IPC_releaseMutex() - Ok" << endl;
}

    res = IPC_deleteMutex(lockMutex);
    if(res != IPC_OK) {
        std::cout << "IPC_deleteMutex() - Error" << endl;
    } else {

    std::cout << "IPC_deleteMutex() - Ok" << endl;
}

    IPC_cleanup();

    std::cout << "TEST COMPLETE" << endl;

    return 0;
}

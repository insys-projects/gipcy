
#ifdef __IPC_WIN__
#include "winipc.h"
#endif

#include "ipc.h"

#include <iostream>
#include <string>
#include <istream>
#include <fstream>
#include <iomanip>
#include <cstring>

#include <signal.h>

using namespace std;

//------------------------------------------------------------------
#define LIB_NAME "../../bin/libdevreg.so"
//------------------------------------------------------------------

int main(int argc, char* argv[])
{
    std::cout << "GIPCY LIBRARY TEST START" << endl;

    IPC_handle hlib = IPC_openLibrary(LIB_NAME, 0);
    if(!hlib) {
        std::cout << "Error open library " << LIB_NAME << endl;
        return -1;
    }

    std::cout << "Open library " << LIB_NAME << endl;

    IPC_closeLibrary(hlib);

    std::cout << "Close library " << LIB_NAME << endl;

    return 0;
}

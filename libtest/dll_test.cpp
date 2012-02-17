
#ifdef __IPC_WIN__
#include "winipc.h"
#endif

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
#define LIB_NAME1 "../../bin/libbrd.so"
#define LIB_NAME2 "../../bin/libdevreg.so"
#define LIB_NAME3 "../../bin/libbaseres.so"
#define LIB_NAME4 "../../bin/libm214x500m.so"
#define LIB_NAME5 "../../bin/libbambpex.so"
//------------------------------------------------------------------

int test_lib(const IPC_str *libName )
{
    IPC_handle hlib = IPC_openLibrary(libName, 0);
    if(!hlib) {
        std::cout << "Error open library " << libName << endl;
        return 1;
    }

    std::cout << "Open library " << libName << " - success" << endl;

    IPC_closeLibrary(hlib);

    return 0;
}

//------------------------------------------------------------------

int main(int argc, char* argv[])
{
    test_lib(LIB_NAME1);
    test_lib(LIB_NAME2);
    test_lib(LIB_NAME3);
    test_lib(LIB_NAME4);
    test_lib(LIB_NAME5);

    return 0;
}

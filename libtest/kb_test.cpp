
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

int main(int argc, char* argv[])
{
    IPC_initKeyboard();

    while(!IPC_kbhit());

    IPC_getch();

    IPC_cleanupKeyboard();

    return 0;
}

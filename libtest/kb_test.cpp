
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

    int key_code = -1;

    while(1) {

        if(IPC_kbhit()) {
            key_code = IPC_getch();
            fprintf(stdout, "Pressed key code: %d\n", key_code);
        }

        if(key_code == 27)
            break;
        IPC_delay(10);
    };

    fprintf(stdout, "Press any key for exit...\n");
    IPC_getch();
    IPC_cleanupKeyboard();

    return 0;
}

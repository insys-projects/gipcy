
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
//! Placed in shared memory

struct test_param {

    unsigned long exit_flag;
    IPC_handle	  lock;
    unsigned long write_counter;
    unsigned long read_counter;
    unsigned long *mem_addr;
    unsigned long mem_size;
};

//------------------------------------------------------------------
#define ROW 8
#define COL 16
#define MEM_SIZE ROW*COL
//------------------------------------------------------------------

int main(int argc, char* argv[])
{
    int wasCreated = 0;
    IPC_handle shm = IPC_createSharedMemoryEx("sharedMemory", MEM_SIZE, &wasCreated);
    if(!shm) {
        std::cout << "IPC_createSharedMemoryEx() failed" << endl;
        return -1;
    }

    if(wasCreated) {
        std::cout << "Open shared memory...Ok" << endl;
    } else {
        std::cout << "Create shared memory...Ok" << endl;
    }

    unsigned long *pmem = (unsigned long *)IPC_mapSharedMemory(shm);
    if(!pmem) {
        std::cout << "IPC_mapSharedMemory() failed" << endl;
        IPC_deleteSharedMemory(shm);
        return -1;
    }
    std::cout << "Map shared memory...Ok" << endl;

    if(wasCreated) {

        std::cout << "Press enter to read shared memory" << endl;
        getchar();

    } else {

        std::cout << "Press enter to fill shared memory" << endl;
        getchar();
        memset(pmem, 0x55, MEM_SIZE);
    }


    std::cout << "Shared memory data:" << endl;
    std::cout << endl;

    unsigned char *m = (unsigned char *)pmem;
    for(unsigned i=0; i<ROW; i++) {
        std::cout << hex << i << ": " << ends;
        for(unsigned j=0; j<COL; j++) {
            std::cout << hex << int(m[i*ROW+j]) << " " << ends;
        }
        std::cout << endl;
    }
    std::cout << endl;

    IPC_unmapSharedMemory(shm);
    std::cout << "Unmap shared memory...Ok" << endl;

    IPC_deleteSharedMemory(shm);
    std::cout << "Delete shared memory...Ok" << endl;

    return 0;
}

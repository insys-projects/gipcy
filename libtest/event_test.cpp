
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
#define MEM_SIZE 16
//------------------------------------------------------------------

thread_value __IPC_API reading_thread(void* param)
{
    std::cout << "Start reading thread" << endl;

    struct test_param *tp = (struct test_param*)param;
    if(!tp) {
        std::cout << "Invalid parameters" << endl;
        return NULL;
    }

    while(!tp->exit_flag) {

	int res = IPC_waitEvent(tp->lock, -1);
	if(res != IPC_OK)
	    continue;

	tp->read_counter++;

	unsigned char *data = (unsigned char*)tp->mem_addr;
	for(unsigned i=0; i<tp->mem_size; i++) {
	    std::cout << int(data[i]) << " ";
	}
	std::cout << endl;
    }

    std::cout << "Reading thread stoped" << endl;

    return NULL;
}

//------------------------------------------------------------------

static struct test_param tp = {0};
static void exit_application( int ) 
{
    //std::cout << endl;
    tp.exit_flag = 1; 
    //std::cout << endl;
}

//------------------------------------------------------------------

int main(int argc, char* argv[])
{
    std::cout << "GIPCY LIBRARY TEST START" << endl;
    
    signal(SIGINT, exit_application);

    IPC_init();

    std::cout << "Create synchronize event..." << endl;
    IPC_handle lockEv = IPC_createEvent("lockEv", true, true);
    if(!lockEv) {
        std::cout << "IPC_createSemaphore() failed" << endl;
        return -1;
    }
    std::cout << "Ok" << endl;

    std::cout << "Create shared memory..." << endl;
    int wasCreated = 0;
    IPC_handle shm = IPC_createSharedMemoryEx("sharedMemory", MEM_SIZE, &wasCreated);
    if(!shm) {
        std::cout << "IPC_createSharedMemoryEx() failed" << endl;
        IPC_deleteEvent(lockEv);
        return -1;
    }
    std::cout << "Ok" << endl;

    std::cout << "Map shared memory..." << endl;
    unsigned long *pmem = (unsigned long *)IPC_mapSharedMemory(shm);
    if(!pmem) {
        std::cout << "IPC_mapSharedMemory() failed" << endl;
        IPC_deleteEvent(lockEv);
        IPC_deleteSharedMemory(shm);
        return -1;
    }
    std::cout << "Ok" << endl;
    
    memset(&tp, 0, sizeof(struct test_param));

    tp.lock = lockEv;
    tp.mem_addr = pmem;
    tp.mem_size = MEM_SIZE;

    std::cout << "Create reading thread..." << endl;
    IPC_handle threadR = IPC_createThread("thread_r", reading_thread, &tp);
    if(!threadR) {
        std::cout << "IPC_createThread() failed create reading thread" << endl;
        IPC_unmapSharedMemory(shm);
        IPC_deleteEvent(lockEv);
        IPC_deleteSharedMemory(shm);
        return -1;
    }
    std::cout << "Ok" << endl;

    std::cout << "Press Ctrl+C to exit..." << endl;

    unsigned char cycle = 0;

    while(1)
	{
		if(tp.exit_flag)
			break;
		IPC_delay(1000);
                memset(pmem, cycle++, MEM_SIZE);
                if(IPC_setEvent(lockEv) < 0)
                    break;
	}
    std::cout << endl;

    std::cout << "Delete reading thread..." << endl;
    IPC_deleteThread(threadR);
    std::cout << "Ok" << endl;
    
    std::cout << "Unmap shared memory..." << endl;
    IPC_unmapSharedMemory(shm);
    std::cout << "Ok" << endl;

    std::cout << "Delete shared memory..." << endl;
    IPC_deleteSharedMemory(shm);
    std::cout << "Ok" << endl;

    std::cout << "Delete event..." << endl;
    IPC_deleteEvent(lockEv);
    std::cout << "Ok" << endl;
    
    std::cout << "GIPCY LIBRARY TEST COMPLETE" << endl;

    IPC_cleanup();

    return 0;
}

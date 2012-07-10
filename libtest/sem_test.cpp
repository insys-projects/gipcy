
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

thread_value __IPC_API writing_thread(void* param)
{
    //std::cout << "Start writing thread" << endl;

    struct test_param *tp = (struct test_param*)param;
    if(!tp) {
        std::cout << "Invalid parameters" << endl;
        return NULL;
    }

    while(!tp->exit_flag) {

	int res = IPC_lockSemaphore(tp->lock, -1);
	//int res = IPC_captureMutex(tp->lock, -1);
	if(res != IPC_OK)
	    break;
	//std::cout << "writing_tread(): lock" << endl;
	//std::cout << "writing_tread(): write data" << endl;
	tp->write_counter++;
	memset(tp->mem_addr,tp->write_counter,tp->mem_size);
	//std::cout << "writing_thread(): W " << tp->write_counter << " R: " << tp->read_counter  << endl;

	IPC_delay(1000);

	//std::cout << "writing_tread(): unlock" << endl;
	res = IPC_unlockSemaphore(tp->lock);
	//res = IPC_releaseMutex(tp->lock);
	if(res != IPC_OK)
	    break;
    }

    //std::cout << "Writing thread stoped" << endl;

    return NULL;
}

//------------------------------------------------------------------

thread_value __IPC_API reading_thread(void* param)
{
    //std::cout << "Start reading thread" << endl;

    struct test_param *tp = (struct test_param*)param;
    if(!tp) {
        std::cout << "Invalid parameters" << endl;
        return NULL;
    }

    while(!tp->exit_flag) {
	int res = IPC_lockSemaphore(tp->lock, -1);
	//int res = IPC_captureMutex(tp->lock, -1);
	if(res != IPC_OK)
	    break;
	//std::cout << "reading_tread(): lock" << endl;
	//std::cout << "reading_tread(): read data" << endl;
	tp->read_counter++;
	unsigned char *data = (unsigned char*)tp->mem_addr;
	for(unsigned i=0; i<tp->mem_size; i++) {
	    std::cout << int(data[i]) << " ";
	}
	std::cout << endl;
	//std::cout << "reading_thread(): W " << tp->write_counter << " R: " << tp->read_counter  << endl;
	//std::cout << "reading_tread(): unlock" << endl;
	res = IPC_unlockSemaphore(tp->lock);
	//res = IPC_releaseMutex(tp->lock);
	if(res != IPC_OK)
	    break;
    }

    //std::cout << "Reading thread stoped" << endl;

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

    std::cout << "Create synchronize semaphore..." << endl;
    IPC_handle lockSem = IPC_createSemaphore("lockSem", 1);
    if(!lockSem) {
        std::cout << "IPC_createSemaphore() failed" << endl;
        return -1;
    }
    //std::cout << "Create synchronize mutex..." << endl;
    //IPC_handle captMutex = IPC_createMutex("captMutex", false);
    //if(!captMutex) {
    //    std::cout << "IPC_createMutex() failed" << endl;
    //    return -1;
    //}
    std::cout << "Ok" << endl;

    std::cout << "Create shared memory..." << endl;
    int wasCreated = 0;
    IPC_handle shm = IPC_createSharedMemoryEx("sharedMemory", MEM_SIZE, &wasCreated);
    if(!shm) {
        std::cout << "IPC_createSharedMemoryEx() failed" << endl;
        IPC_deleteSemaphore(lockSem);
        //IPC_deleteMutex(captMutex);
        return -1;
    }
    std::cout << "Ok" << endl;

    std::cout << "Map shared memory..." << endl;
    unsigned long *pmem = (unsigned long *)IPC_mapSharedMemory(shm);
    if(!pmem) {
        std::cout << "IPC_mapSharedMemory() failed" << endl;
        IPC_deleteSemaphore(lockSem);
        //IPC_deleteMutex(captMutex);
        IPC_deleteSharedMemory(shm);
        return -1;
    }
    std::cout << "Ok" << endl;

    memset(&tp, 0, sizeof(struct test_param));

    tp.lock = lockSem;
    //tp.lock = captMutex;
    tp.mem_addr = pmem;
    tp.mem_size = MEM_SIZE;
    tp.exit_flag = 0;

    std::cout << "Create writing thread..." << endl;
    IPC_handle threadW = IPC_createThread("thread_w", writing_thread, &tp);
    if(!threadW) {
        std::cout << "IPC_createThread() failed create writing thread" << endl;
        IPC_unmapSharedMemory(shm);
        IPC_deleteSemaphore(lockSem);
        //IPC_deleteMutex(captMutex);
        IPC_deleteSharedMemory(shm);
        return -1;
    }
    std::cout << "Ok" << endl;

    std::cout << "Create reading thread..." << endl;
    IPC_handle threadR = IPC_createThread("thread_r", reading_thread, &tp);
    if(!threadR) {
        std::cout << "IPC_createThread() failed create reading thread" << endl;
        IPC_deleteThread(threadW);
        IPC_unmapSharedMemory(shm);
        IPC_deleteSemaphore(lockSem);
        //IPC_deleteMutex(captMutex);
        IPC_deleteSharedMemory(shm);
        return -1;
    }
    std::cout << "Ok" << endl;

    std::cout << "Press Ctrl+C to exit..." << endl;

    while(1)
	{
		if(tp.exit_flag)
			break;
		IPC_delay(1000);
	}
    std::cout << endl;

    std::cout << "Delete reading thread..." << endl;
    IPC_deleteThread(threadR);
    std::cout << "Ok" << endl;
    
    std::cout << "Delete writing thread..." << endl;
    IPC_deleteThread(threadW);
    std::cout << "Ok" << endl;
    
    std::cout << "Write counter: " << tp.write_counter << " Read counter: " << tp.read_counter  << endl;

    std::cout << "Unmap shared memory..." << endl;
    IPC_unmapSharedMemory(shm);
    std::cout << "Ok" << endl;

    std::cout << "Delete shared memory..." << endl;
    IPC_deleteSharedMemory(shm);
    std::cout << "Ok" << endl;

    std::cout << "Delete semaphore..." << endl;
    IPC_deleteSemaphore(lockSem);
        //std::cout << "Delete mutex..." << endl;
	//IPC_deleteMutex(captMutex);
    std::cout << "Ok" << endl;
    
    std::cout << "GIPCY LIBRARY TEST COMPLETE" << endl;

    return 0;
}


#include "gipcy.h"

#include <iostream>
#include <string>
#include <istream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <csignal>
#include <memory>
#include <thread>
#include <vector>
#include <mutex>

using namespace std;

//------------------------------------------------------------------

class Gipcy
{
private:
    Gipcy() {
        _ok = false;
        int err = IPC_init();
        if(err < 0) {
            fprintf(stdout,"%s, %d: %s() - Erorr in IPC_init()!\n", __FILE__, __LINE__, __FUNCTION__);
            _ok = false;
        } else {
            _ok = true;
            fprintf(stdout,"%s, %d: %s() - IPC_init() - OK\n", __FILE__, __LINE__, __FUNCTION__);
        }
    }
    Gipcy(const Gipcy&);
    Gipcy& operator=(Gipcy&);
    bool _ok;
public:
    static bool initGipcy() {
        static Gipcy gipcy;
        return gipcy.init_ok();
    }
    virtual ~Gipcy() {
        IPC_cleanup();
        fprintf(stdout,"%s, %d: %s() - IPC_cleanup() - OK\n", __FILE__, __LINE__, __FUNCTION__);
    }
    bool init_ok() {
        return _ok;
    }
};

//-----------------------------------------------------------------------------

class ipc_mutex {
public:
    ipc_mutex(const std::string& name, int value) {
        _mutex = IPC_createMutex(name.c_str(), value);
        if(!_mutex) {
            fprintf(stderr, "Error in IPC_createMutex(%s)\n", name.c_str());
            exit(-1);
        }
    }
    virtual ~ipc_mutex() {
        if(_mutex) IPC_deleteMutex(_mutex);
    }
    bool lock(int timeout = -1) {
        int res = IPC_captureMutex(_mutex, timeout);
        if(res != IPC_OK)
            return false;
        return true;
    }
    bool unlock() {
        int res = IPC_releaseMutex(_mutex);
        if(res != IPC_OK)
            return false;
        return true;
    }
private:
    IPC_handle _mutex;
};

//-----------------------------------------------------------------------------

void mutex_test(const std::string& name, int id)
{
    int lock_counter = 0;
    ipc_mutex m(name, 1);
    while(1) {
        fprintf(stderr, "%d: - try\n", id);
        if(m.lock()) {
            //std::this_thread::sleep_for(50ms);
            fprintf(stderr, "%d: - lock\n", id);
            ++lock_counter;
            m.unlock();
            fprintf(stderr, "%d: - unlock\n", id);
            //std::this_thread::sleep_for(20ms);
        }
        if(lock_counter > 1000) {
            break;
        }
    }
    fprintf(stderr, "%d: lock_counter = %d [FINISH]\n", id, lock_counter);
}

//-----------------------------------------------------------------------------

using job_t = shared_ptr<std::thread>;

inline job_t create_job(const std::string& name, int id)
{
    return std::make_shared<std::thread>(mutex_test, name, id);
}

//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    if(!Gipcy::initGipcy()) {
        fprintf(stderr, "Gipcy::initGipcy() - Error.\n");
        return -1;
    }

    std::vector<job_t> jobs;
    for(unsigned ii=0; ii<5; ++ii) {
        jobs.push_back(create_job("_testMutex", ii));
    }

    for(auto& job : jobs) {
        job->join();
    }

    fprintf(stderr, "Test complete!\n");

    return 0;
}

//-----------------------------------------------------------------------------

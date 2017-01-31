
#ifdef __IPC_LINUX__

#ifndef __LINIPC_H__
#include "linipc.h"
#endif
#ifndef __GIPCY_H__
    #include "gipcy.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <error.h>
#include <time.h>

//-----------------------------------------------------------------------------

IPC_handle IPC_createSemaphore(const IPC_str *name, int value)
{
    ipc_handle_t h = allocate_ipc_object(name, IPC_typeSemaphore);
    if(!h)
        return NULL;

    h->ipc_data = NULL;

    h->ipc_descr.ipc_sem = sem_open(name, IPC_CREAT | IPC_EXCL, 0666, value);
    if(h->ipc_descr.ipc_sem == SEM_FAILED) {

        h->ipc_descr.ipc_sem = sem_open(name, O_CREAT, 0666, value);
        if(h->ipc_descr.ipc_sem == SEM_FAILED) {
            DEBUG_PRINT("%s(): error open semaphore - %s. %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
            delete_ipc_object(h);
            return NULL;
        } else {
            DEBUG_PRINT("%s(): semaphore - %s opened\n", __FUNCTION__, h->ipc_name);
        }

    } else {

        DEBUG_PRINT("%s(): semaphore - %s created\n", __FUNCTION__, h->ipc_name);
    }

    return h;
}

//-----------------------------------------------------------------------------

int IPC_lockSemaphore(const  IPC_handle handle, int timeout)
{
    if(!handle)
        return IPC_INVALID_HANDLE;

    ipc_handle_t h = (ipc_handle_t)handle;

    DEBUG_PRINT("%s(): wait semaphore - %s started...\n", __FUNCTION__, h->ipc_name);

    int res = 0;
    if(timeout > 0) {
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = timeout*10000;
        res = sem_timedwait(h->ipc_descr.ipc_sem, &ts);
    } else {
        res = sem_wait(h->ipc_descr.ipc_sem);
    }

    if(res < 0) {
        if(errno == ETIMEDOUT) {
            DEBUG_PRINT("%s(): wait semaphore %s timeout\n", __FUNCTION__, h->ipc_name);
            return IPC_WAIT_TIMEOUT;
        } else {
            DEBUG_PRINT("%s(): wait semaphore %s error - %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
            return IPC_GENERAL_ERROR;
        }
    }

    DEBUG_PRINT("%s(): wait semaphore %s Ok\n", __FUNCTION__, h->ipc_name);

    return IPC_OK;
}

//-----------------------------------------------------------------------------

int IPC_unlockSemaphore(const  IPC_handle handle)
{
    if(!handle)
        return IPC_INVALID_HANDLE;

    ipc_handle_t h = (ipc_handle_t)handle;

    int res = sem_post(h->ipc_descr.ipc_sem);
    if(res < 0) {
        if(errno == EINTR) {
            DEBUG_PRINT("%s(): Posting was interrputed - %s\n", __FUNCTION__, h->ipc_name);
            return IPC_GENERAL_ERROR;
        }
        DEBUG_PRINT("%s(): %s - %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
        return IPC_GENERAL_ERROR;
    }

    DEBUG_PRINT("%s(): semaphore - %s unlocked\n", __FUNCTION__, h->ipc_name);

    return IPC_OK;
}

//-----------------------------------------------------------------------------

int IPC_deleteSemaphore(IPC_handle handle)
{
    if(!handle)
        return IPC_INVALID_HANDLE;

    ipc_handle_t h = (ipc_handle_t)handle;

    if(h->ipc_type != IPC_typeSemaphore)
        return IPC_INVALID_HANDLE;

    int res = sem_close(h->ipc_descr.ipc_sem);
    if(res < 0) {
        DEBUG_PRINT("%s(): close semaphore error - %s\n", __FUNCTION__, strerror(errno));
        return IPC_GENERAL_ERROR;
    }

    res = sem_unlink(h->ipc_name);
    if(res < 0) {
        DEBUG_PRINT("%s(): unlink semaphore %s error - %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
        return IPC_GENERAL_ERROR;
    }

    DEBUG_PRINT("%s(): semaphore - %s deleted\n", __FUNCTION__, h->ipc_name);

    delete_ipc_object((ipc_handle_t)handle);

    DEBUG_PRINT("%s(): semaphore deleted successfully\n", __FUNCTION__);

    return IPC_OK;
}

//-----------------------------------------------------------------------------

#endif //__IPC_LINUX__

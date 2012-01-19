
#ifdef __IPC_LINUX__

#ifndef __LINIPC_H__
#include "linipc.h"
#endif
#ifndef __MUTEXIPC_H__
#include "mutexipc.h"
#endif
#ifndef __SYSIPC_H__
#include "sysipc.h"
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

IPC_handle IPC_createMutex(const IPC_str *name, bool value)
{
    ipc_handle_t h = allocate_ipc_object(name, IPC_typeMutex);
    if(!h)
        return NULL;

    h->ipc_data = NULL;

    DEBUG_PRINT("%s(): %s - %d\n", __FUNCTION__, h->ipc_name, value);

    h->ipc_descr.ipc_sem = sem_open(name, O_CREAT | O_EXCL, 0666, (value) ? 0 : 1);
    if(h->ipc_descr.ipc_sem == SEM_FAILED) {

        h->ipc_descr.ipc_sem = sem_open(name, O_CREAT, 0666, (value) ? 0 : 1);
        if(h->ipc_descr.ipc_sem == SEM_FAILED) {
            DEBUG_PRINT("%s(): error open mutex - %s. %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
            delete_ipc_object(h);
            return NULL;
        } else {
            DEBUG_PRINT("%s(): mutex - %s opened\n", __FUNCTION__, h->ipc_name);
        }

    } else {

        DEBUG_PRINT("%s(): mutex - %s created\n", __FUNCTION__, h->ipc_name);
    }

    return h;
}

//-----------------------------------------------------------------------------

int IPC_captureMutex(const  IPC_handle handle, int timeout)
{
    if(!handle)
        return IPC_invalidHandle;

    ipc_handle_t h = (ipc_handle_t)handle;

    //DEBUG_PRINT("%s(): capture mutex - %s started...\n", __FUNCTION__, h->ipc_name);

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
            DEBUG_PRINT("%s(): capture mutex %s timeout\n", __FUNCTION__, h->ipc_name);
            return IPC_timeout;
        } else {
            DEBUG_PRINT("%s(): capture mutex %s error - %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
            return IPC_generalError;
        }
    }

    //DEBUG_PRINT("%s(): capture mutex %s Ok\n", __FUNCTION__, h->ipc_name);

    return IPC_ok;
}

//-----------------------------------------------------------------------------

int IPC_releaseMutex(const  IPC_handle handle)
{
    if(!handle)
        return IPC_invalidHandle;

    ipc_handle_t h = (ipc_handle_t)handle;

    int res = sem_post(h->ipc_descr.ipc_sem);
    if(res < 0) {
        DEBUG_PRINT("%s(): release mutex %s error - %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
        return IPC_generalError;
    }

    //DEBUG_PRINT("%s(): release mutex %s Ok\n", __FUNCTION__, h->ipc_name);

    return IPC_ok;
}

//-----------------------------------------------------------------------------

int IPC_deleteMutex(IPC_handle handle)
{
    if(!handle)
        return IPC_invalidHandle;

    ipc_handle_t h = (ipc_handle_t)handle;

    if(h->ipc_type != IPC_typeMutex)
        return IPC_invalidHandle;

    int res = sem_close(h->ipc_descr.ipc_sem);
    if(res < 0) {
        DEBUG_PRINT("%s(): close mutex error - %s\n", __FUNCTION__, strerror(errno));
        return IPC_generalError;
    }

    res = sem_unlink(h->ipc_name);
    if(res < 0) {
        DEBUG_PRINT("%s(): unlink mutex %s error - %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
        return IPC_generalError;
    }

    DEBUG_PRINT("%s(): mutex - %s deleted\n", __FUNCTION__, h->ipc_name);

    delete_ipc_object((ipc_handle_t)handle);

    DEBUG_PRINT("%s(): mutex deleted successfully\n", __FUNCTION__);

    return IPC_ok;
}

//-----------------------------------------------------------------------------

#endif //__IPC_LINUX__

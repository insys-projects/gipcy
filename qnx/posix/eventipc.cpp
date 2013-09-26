
#ifdef __IPC_QNX__

#ifndef __QNXIPC_H__
#include "qnxipc.h"
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
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/types.h>

//-----------------------------------------------------------------------------
//  manual = TRUE - manual reset, FALSE - autoreset
//  value = TRUE - начальное состояние Signaled
IPC_handle IPC_createEvent(const IPC_str *name, bool manual, bool value)
{
    ipc_handle_t h = allocate_ipc_object(name, IPC_typeEvent);
    if(!h)
        return NULL;

    if(manual) {
    } else {
    }

    h->ipc_descr.ipc_sem = sem_open(h->ipc_name, O_CREAT | O_EXCL, 0666, (value) ? 1 : 0 );
    if(h->ipc_descr.ipc_sem == SEM_FAILED) {
        if(errno != EEXIST) {
            DEBUG_PRINT("%s(): error open event - %s. %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
            delete_ipc_object(h);
            return NULL;
        } else {
            h->ipc_descr.ipc_sem = sem_open(name, O_CREAT, 0666, (value) ? 1 : 0);
            if(h->ipc_descr.ipc_sem == SEM_FAILED) {
                DEBUG_PRINT("%s(): error open event - %s. %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
                delete_ipc_object(h);
                return NULL;
            } else {
                DEBUG_PRINT("%s(): event - %s opened\n", __FUNCTION__, h->ipc_name);
            }
        }
    } else {

        DEBUG_PRINT("%s(): event - %s created\n", __FUNCTION__, h->ipc_name);
    }

    return h;
}

//-----------------------------------------------------------------------------

int IPC_waitEvent(const  IPC_handle handle, int timeout)
{
    if(!handle)
        return IPC_INVALID_HANDLE;

    ipc_handle_t h = (ipc_handle_t)handle;

    DEBUG_PRINT("%s(): wait event - %s started...\n", __FUNCTION__, h->ipc_name);

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
            DEBUG_PRINT("%s(): wait event %s timeout\n", __FUNCTION__, h->ipc_name);
            return IPC_WAIT_TIMEOUT;
        } else {
            DEBUG_PRINT("%s(): wait event %s error - %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
            return IPC_GENERAL_ERROR;
        }
    }

    DEBUG_PRINT("%s(): wait event %s Ok\n", __FUNCTION__, h->ipc_name);

    return IPC_OK;
}

//-----------------------------------------------------------------------------

int IPC_setEvent(const  IPC_handle handle)
{
    if(!handle)
        return IPC_INVALID_HANDLE;

    ipc_handle_t h = (ipc_handle_t)handle;

    int res = sem_post(h->ipc_descr.ipc_sem);
    if(res < 0) {
        DEBUG_PRINT("%s(): wait event %s error - %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
        return IPC_GENERAL_ERROR;
    }

    DEBUG_PRINT("%s(): set event %s Ok\n", __FUNCTION__, h->ipc_name);

    return IPC_OK;
}

//-----------------------------------------------------------------------------

int IPC_resetEvent(const  IPC_handle handle)
{
    if(!handle)
        return IPC_INVALID_HANDLE;

    ipc_handle_t h = (ipc_handle_t)handle;

    int value = 0;
    int res = sem_getvalue(h->ipc_descr.ipc_sem, &value);
    if(res < 0) {
        DEBUG_PRINT("%s(): get event value error - %s\n", __FUNCTION__, strerror(errno));
        return IPC_GENERAL_ERROR;
    }

    if(value > 0) {
        res = sem_wait(h->ipc_descr.ipc_sem);
        if(res < 0) {
            DEBUG_PRINT("%s(): wait event %s error - %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
            return IPC_GENERAL_ERROR;
        }
    }

    DEBUG_PRINT("%s(): reset event %s Ok\n", __FUNCTION__, h->ipc_name);

    return IPC_OK;
}

//-----------------------------------------------------------------------------

int IPC_deleteEvent(IPC_handle handle)
{
    if(!handle)
        return IPC_INVALID_HANDLE;

    ipc_handle_t h = (ipc_handle_t)handle;

    if(h->ipc_type != IPC_typeEvent)
        return IPC_INVALID_HANDLE;

    int res = sem_close(h->ipc_descr.ipc_sem);
    if(res < 0) {
        DEBUG_PRINT("%s(): close event error - %s\n", __FUNCTION__, strerror(errno));
        return IPC_GENERAL_ERROR;
    }

    res = sem_unlink(h->ipc_name);
    if(res < 0) {
        DEBUG_PRINT("%s(): unlink event %s error - %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
        return IPC_GENERAL_ERROR;
    }

    DEBUG_PRINT("%s(): event - %s deleted\n", __FUNCTION__, h->ipc_name);

    delete_ipc_object((ipc_handle_t)handle);

    DEBUG_PRINT("%s(): event deleted successfully\n", __FUNCTION__);

    return IPC_OK;
}

//-----------------------------------------------------------------------------

#endif //__IPC_QNX__

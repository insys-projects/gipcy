

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
#include <errno.h>
#include <error.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>
#include <limits.h>

//-----------------------------------------------------------------------------

IPC_handle IPC_createThread(const IPC_str *name, thread_func *function, void* param)
{
    if(!function)
        return NULL;

    ipc_handle_t h = allocate_ipc_object(name, IPC_typeThread);
    if(!h)
        return NULL;

    h->ipc_data = NULL;
    h->ipc_size = 0;

    int res = pthread_create(&h->ipc_descr.ipc_thread,NULL,function,param);
    if(res < 0) {
        DEBUG_PRINT("%s(): error create thread. %s\n", __FUNCTION__, strerror(errno));
        delete_ipc_object(h);
        return NULL;
    }

    DEBUG_PRINT("%s(): thread %s was created\n", __FUNCTION__, h->ipc_name);

    return h;
}

//-----------------------------------------------------------------------------

IPC_handle IPC_createThreadEx(const IPC_str *name, struct thread_param *tp, int flags)
{
    if(!tp)
        return NULL;

    if(!tp->threadFunction || !tp->threadMutex)
        return NULL;

    ipc_handle_t h = allocate_ipc_object(name, IPC_typeThread);
    if(!h)
        return NULL;

    h->ipc_data = tp;
    h->ipc_size = sizeof(struct thread_param);

    if(flags & CREATE_THREAD_PAUSED) {
        IPC_lockSemaphore(tp->threadMutex, 0);
    }

    int res = pthread_create(&h->ipc_descr.ipc_thread,NULL,tp->threadFunction,tp);
    if(res < 0) {
        DEBUG_PRINT("%s(): error create thread. %s\n", __FUNCTION__, strerror(errno));
        delete_ipc_object(h);
        return NULL;
    }

    DEBUG_PRINT("%s(): thread %s was created\n", __FUNCTION__, h->ipc_name);

    return h;
}

//-----------------------------------------------------------------------------

int IPC_startThread(const IPC_handle handle)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h || h->ipc_type != IPC_typeThread)
        return -1;

    struct thread_param *tp = (struct thread_param *)h->ipc_data;

    if(!tp)
        return -1;

    if(tp->threadMutex) {
        IPC_unlockSemaphore(tp->threadMutex);
        DEBUG_PRINT("%s(): thread %s was started\n", __FUNCTION__, h->ipc_name);
    }

    return IPC_ok;
}

//-----------------------------------------------------------------------------

int IPC_stopThread(const IPC_handle handle)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h || h->ipc_type != IPC_typeThread) return -EINVAL;

    int res = pthread_cancel(h->ipc_descr.ipc_thread);
    if(res < 0) {
        DEBUG_PRINT("%s(): thread %s was error %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
        return -1;
    }

    DEBUG_PRINT("%s(): thread %s was canceled\n", __FUNCTION__, h->ipc_name);

    return IPC_ok;
}

//-----------------------------------------------------------------------------

int IPC_waitThread(const IPC_handle handle, int timeout)
{
    if(!handle) {
        int res = pthread_join(0,NULL);
	if(res < 0) {
            DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno));
    	    return -1;
	}
	return IPC_ok;
    }

    ipc_handle_t h = (ipc_handle_t)handle;
    if(h->ipc_type != IPC_typeThread) return -EINVAL;
    void *ret = 0;

    int res = pthread_join(h->ipc_descr.ipc_thread, &ret);
    if(res < 0) {
        DEBUG_PRINT("%s(): thread %s was error %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
        return -1;
    }

    DEBUG_PRINT("%s(): thread %s was finished\n", __FUNCTION__, h->ipc_name);

    return IPC_ok;
}

//-----------------------------------------------------------------------------

int IPC_deleteThread(IPC_handle handle)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h || h->ipc_type != IPC_typeThread) return -EINVAL;

    void *ret = 0;

    int res = pthread_join(h->ipc_descr.ipc_thread, &ret);
    if(res < 0) {
        if(res != ESRCH) {
            DEBUG_PRINT("%s(): thread %s was error %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
            return -1;
        }
    }

    DEBUG_PRINT("%s(): thread %s was deleted\n", __FUNCTION__, h->ipc_name);

    delete_ipc_object(h);

    return IPC_ok;
}

//-----------------------------------------------------------------------------

#endif //__IPC_LINUX__

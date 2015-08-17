

#ifdef __IPC_QNX__

#ifndef __QNX_H__
    #include "qnxipc.h"
#endif
#ifndef __GIPCY_H__
    #include "gipcy.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>
#include <limits.h>

//-----------------------------------------------------------------------------

GIPCY_API    IPC_handle IPC_createThread(const IPC_str *name, thread_func* function, void* param)
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

GIPCY_API IPC_handle IPC_createThreadEx(const IPC_str *name, struct thread_param *tp, int flags)
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

    if(flags & IPC_CREATE_THREAD_PAUSED) {
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

GIPCY_API int IPC_startThread(const IPC_handle handle)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h || h->ipc_type != IPC_typeThread)
        return IPC_INVALID_HANDLE;

    struct thread_param *tp = (struct thread_param *)h->ipc_data;

    if(!tp)
        return IPC_GENERAL_ERROR;

    if(tp->threadMutex) {
        IPC_unlockSemaphore(tp->threadMutex);
        DEBUG_PRINT("%s(): thread %s was started\n", __FUNCTION__, h->ipc_name);
    }

    return IPC_OK;
}

//-----------------------------------------------------------------------------

GIPCY_API int IPC_stopThread(const IPC_handle handle)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h || h->ipc_type != IPC_typeThread)
		return IPC_INVALID_HANDLE;

    int res = pthread_cancel(h->ipc_descr.ipc_thread);
    if(res < 0) {
        DEBUG_PRINT("%s(): thread %s was error %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
        return IPC_GENERAL_ERROR;
    }

    DEBUG_PRINT("%s(): thread %s was canceled\n", __FUNCTION__, h->ipc_name);

    return IPC_OK;
}

//-----------------------------------------------------------------------------

GIPCY_API int IPC_waitThread(const IPC_handle handle, int timeout)
{
    if(!handle) {
        int res = pthread_join(0,NULL);
        if(res > 0) {
                DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno));
                return IPC_GENERAL_ERROR;
        }
        return IPC_OK;
    }

    ipc_handle_t h = (ipc_handle_t)handle;
    if(h->ipc_type != IPC_typeThread)
        return IPC_INVALID_HANDLE;

    void *retval = NULL;
    int res = 0;

    if(timeout <= 0) {

        DEBUG_PRINT("%s(): Start waiting...\n", __FUNCTION__);
        res = pthread_join(h->ipc_descr.ipc_thread, &retval);

    } else {

        struct timespec ts;

        if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {

            DEBUG_PRINT("%s(): pthread_join() error int clock_gettime(). Try again.\n", __FUNCTION__);
            return -EAGAIN;
        }

        ts.tv_nsec += (timeout*1000000);
        res = pthread_timedjoin(h->ipc_descr.ipc_thread, NULL, &ts);
    }

    if(res != 0) {

        if(res == ETIMEDOUT) {
            DEBUG_PRINT("%s(): pthread_join() error. retval = %p, ETIMEDOUT\n", __FUNCTION__, retval);
        } else if(res == EBUSY) {
            DEBUG_PRINT("%s(): pthread_join() error. retval = %p, EBUSY\n", __FUNCTION__, retval);
        }
        return IPC_GENERAL_ERROR;
    }

    DEBUG_PRINT("%s(): thread %s was finished. retval = %p. OK\n", __FUNCTION__, h->ipc_name, retval);

    return 0;
}

//-----------------------------------------------------------------------------
/*
GIPCY_API int IPC_waitThread(const IPC_handle handle, int timeout)
{
    if(!handle)
	{
        int res = pthread_join(0,NULL);
		if(res > 0)
		{
				DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno));
    			return IPC_GENERAL_ERROR;
		}
		return IPC_OK;
    }

    ipc_handle_t h = (ipc_handle_t)handle;
    if(h->ipc_type != IPC_typeThread) 
		return IPC_INVALID_HANDLE;
    void *ret = 0;

    int res = pthread_join(h->ipc_descr.ipc_thread, &ret);
    if(res > 0) {
        DEBUG_PRINT("%s(): thread %s was error %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
        return IPC_GENERAL_ERROR;
    }

    DEBUG_PRINT("%s(): thread %s was finished\n", __FUNCTION__, h->ipc_name);

    return IPC_OK;
}
*/
//-----------------------------------------------------------------------------

GIPCY_API int IPC_deleteThread(IPC_handle handle)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h || h->ipc_type != IPC_typeThread)
		return IPC_INVALID_HANDLE;

    void *ret = 0;

    int res = pthread_join(h->ipc_descr.ipc_thread, &ret);
    if(res > 0) {
        if(res != ESRCH) {
            DEBUG_PRINT("%s(): thread %s was error %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
            return IPC_GENERAL_ERROR;
        }
    }

    DEBUG_PRINT("%s(): thread %s was deleted\n", __FUNCTION__, h->ipc_name);

    delete_ipc_object(h);

    return IPC_OK;
}

//-----------------------------------------------------------------------------

IPC_tls_key IPC_createTlsKey(void)
{
    pthread_key_t key;

    int res = pthread_key_create(&key, NULL);
    if(res != 0) {
        return (pthread_key_t)0;
    }

    return key;
}

//-----------------------------------------------------------------------------

void* IPC_tlsGetValue(IPC_tls_key key)
{
    return pthread_getspecific(key);
}

//-----------------------------------------------------------------------------

int IPC_tlsSetValue(IPC_tls_key key, void *ptr)
{
    return pthread_setspecific(key, ptr);
}

//-----------------------------------------------------------------------------

int IPC_deleteTlsKey(IPC_tls_key key)
{
     return pthread_key_delete(key);
}

//-----------------------------------------------------------------------------

#endif //__IPC_QNX_

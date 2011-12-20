
#ifdef __IPC_WIN__

#include	<process.h> 

#ifndef __WINIPC_H__
    #include "winipc.h"
#endif
#ifndef __THREADIPC_H__
    #include "threadipc.h"
#endif

//-----------------------------------------------------------------------------
IPC_handle IPC_createThread(const IPC_str *name, thread_func *function, void *param)
{
    if(!function)
        return NULL;

	ipc_handle_t h = allocate_ipc_object(name, IPC_typeThread);
    if(!h)
        return NULL;

    h->ipc_data = NULL;
    h->ipc_size = 0;

	unsigned int threadID;

	h->ipc_descr = (HANDLE)_beginthreadex( NULL, 0, function, param, 0, &threadID );


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

//    if(flags & CREATE_THREAD_PAUSED) {
//        IPC_lockSemaphore(tp->threadMutex, 0);
//    }

	unsigned int threadID;

	h->ipc_descr = (HANDLE)_beginthreadex( NULL, 0, tp->threadFunction, tp, 0, &threadID );

    return h;
}

//-----------------------------------------------------------------------------
int IPC_stopThread(const IPC_handle handle)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h || h->ipc_type != IPC_typeThread) return IPC_invalidHandle;

	TerminateThread(h->ipc_descr, 0);

    return IPC_ok;
}

//-----------------------------------------------------------------------------
int IPC_waitThread(const IPC_handle handle, int timeout)
{
    if(!handle)
        return IPC_invalidHandle;
	ipc_handle_t h = (ipc_handle_t)handle;

    WaitForSingleObject(h->ipc_descr, timeout );

    return IPC_ok;
}

//-----------------------------------------------------------------------------
int IPC_deleteThread(IPC_handle handle)
{
    if(!handle)
        return IPC_invalidHandle;
    ipc_handle_t h = (ipc_handle_t)handle;

	CloseHandle(h->ipc_descr);

    delete_ipc_object(h);
	return IPC_ok;
}

//-----------------------------------------------------------------------------

#endif //__IPC_WIN__

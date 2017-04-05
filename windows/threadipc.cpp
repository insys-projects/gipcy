
#ifdef __IPC_WIN__

#include	<process.h> 

#ifndef __WINIPC_H__
    #include "winipc.h"
#endif
#ifndef __THREADIPC_H__
    #include "threadipc.h"
#endif

//-----------------------------------------------------------------------------
GIPCY_API IPC_handle IPC_createThread(const IPC_str *name, thread_func *function, void *param)
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
	if(h->ipc_descr == NULL)
	{
	    delete_ipc_object(h);
		return NULL;
	}

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

	unsigned int threadID;

	h->ipc_descr = (HANDLE)_beginthreadex( NULL, 0, tp->threadFunction, tp, 0, &threadID );
	if(h->ipc_descr == NULL)
	{
	    delete_ipc_object(h);
		return NULL;
	}

    return h;
}

//-----------------------------------------------------------------------------
GIPCY_API int IPC_stopThread(const IPC_handle handle)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h || h->ipc_type != IPC_typeThread)
		return IPC_INVALID_HANDLE;

	int ret = TerminateThread(h->ipc_descr, 0);
	if(!ret)
	    return IPC_GENERAL_ERROR;

    return IPC_OK;
}

//-----------------------------------------------------------------------------
GIPCY_API int IPC_waitThread(const IPC_handle handle, int timeout)
{
    if(!handle)
        return IPC_INVALID_HANDLE;
	ipc_handle_t h = (ipc_handle_t)handle;

    ULONG status = WaitForSingleObject(h->ipc_descr, timeout );
	if(status == WAIT_TIMEOUT) 
		return IPC_WAIT_TIMEOUT;
	else if(status == WAIT_ABANDONED) 
		return IPC_WAIT_ABANDONED;
	else if(status == WAIT_OBJECT_0) 
		return 0;
	else
		return -1;

    return IPC_OK;
}

//-----------------------------------------------------------------------------
GIPCY_API int IPC_deleteThread(IPC_handle handle)
{
    if(!handle)
        return IPC_INVALID_HANDLE;
    ipc_handle_t h = (ipc_handle_t)handle;

	int ret = CloseHandle(h->ipc_descr);
	if(!ret)
	    return IPC_GENERAL_ERROR;

    delete_ipc_object(h);
	return IPC_OK;
}

//-----------------------------------------------------------------------------

GIPCY_API IPC_tls_key IPC_createTlsKey(void)
{
    return TlsAlloc();
}

//-----------------------------------------------------------------------------

GIPCY_API void* IPC_tlsGetValue(IPC_tls_key key)
{
    return TlsGetValue(key);
}

//-----------------------------------------------------------------------------

GIPCY_API int IPC_tlsSetValue(IPC_tls_key key, void *ptr)
{
    return TlsSetValue(key, ptr);
}

//-----------------------------------------------------------------------------

GIPCY_API int IPC_deleteTlsKey(IPC_tls_key key)
{
     return TlsFree(key);
}

//-----------------------------------------------------------------------------

#endif //__IPC_WIN__

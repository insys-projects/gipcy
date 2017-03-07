
#ifdef __IPC_WIN__
#ifndef __WINIPC_H__
    #include "winipc.h"
#endif
#ifndef __SEMIPC_H__
    #include "semipc.h"
#endif

//-----------------------------------------------------------------------------
#ifdef _UNICODE
IPC_handle IPC_createSemaphore(const wchar_t *name, int value)
{
    ipc_handle_t h = allocate_ipc_object(name, IPC_typeSemaphore);
    if(!h)
        return NULL;

    h->ipc_data = NULL;

    h->ipc_descr = CreateSemaphore(NULL, value, 1, name);
    if(h->ipc_descr == INVALID_HANDLE_VALUE)
	{
        delete_ipc_object(h);
        return NULL;
    }

    return h;
}
#else
IPC_handle IPC_createSemaphore(const char *name, int value)
{
    ipc_handle_t h = allocate_ipc_object(name, IPC_typeSemaphore);
    if(!h)
        return NULL;

    h->ipc_data = NULL;

    h->ipc_descr = CreateSemaphore(NULL, value, 1, name);
    if(h->ipc_descr == INVALID_HANDLE_VALUE)
	{
        delete_ipc_object(h);
        return NULL;
    }

    return h;
}
#endif

//-----------------------------------------------------------------------------

int IPC_lockSemaphore(const  IPC_handle handle, int timeout)
{
    if(!handle)
        return IPC_INVALID_HANDLE;

    ipc_handle_t h = (ipc_handle_t)handle;

	int status = WaitForSingleObject(h->ipc_descr, timeout);
	if(status == WAIT_TIMEOUT) 
		return IPC_GENERAL_ERROR;

    return IPC_OK;
}

//-----------------------------------------------------------------------------

int IPC_unlockSemaphore(const  IPC_handle handle)
{
    if(!handle)
        return IPC_INVALID_HANDLE;

    ipc_handle_t h = (ipc_handle_t)handle;

	long prev_cnt;
	ReleaseSemaphore(h->ipc_descr, 1, &prev_cnt);
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

	CloseHandle(h->ipc_descr);
	
    delete_ipc_object(h);
    return IPC_OK;
}

//-----------------------------------------------------------------------------

#endif //__IPC_WIN__

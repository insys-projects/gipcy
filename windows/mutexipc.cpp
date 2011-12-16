
#ifdef __IPC_WIN__
#ifndef __WINIPC_H__
    #include "winipc.h"
#endif
#ifndef __MUTEXIPC_H__
    #include "mutexipc.h"
#endif
/*
//-----------------------------------------------------------------------------
#ifdef _WIN64
IPC_handle IPC_createMutex(const wchar_t *name, bool value)
{
    ipc_handle_t h = allocate_ipc_object(name, IPC_typeMutex);
    if(!h)
        return NULL;

    h->ipc_data = NULL;

    h->ipc_descr = CreateMutex(NULL, value, name);
    if(h->ipc_descr == INVALID_HANDLE_VALUE)
	{
        delete_ipc_object(h);
        return NULL;
    }

    return h;
}
#else
IPC_handle IPC_createMutex(const char *name, bool value)
{
    ipc_handle_t h = allocate_ipc_object(name, IPC_typeMutex);
    if(!h)
        return NULL;

    h->ipc_data = NULL;

    h->ipc_descr = CreateMutex(NULL, value, name);
    if(h->ipc_descr == INVALID_HANDLE_VALUE)
	{
        delete_ipc_object(h);
        return NULL;
    }

    return h;
}
#endif
*/
IPC_handle IPC_createMutex(const IPC_str *name, bool value)
{
    ipc_handle_t h = allocate_ipc_object(name, IPC_typeMutex);
    if(!h)
        return NULL;

    h->ipc_data = NULL;

    h->ipc_descr = CreateMutex(NULL, value, name);
    if(h->ipc_descr == INVALID_HANDLE_VALUE)
	{
        delete_ipc_object(h);
        return NULL;
    }

    return h;
}
//-----------------------------------------------------------------------------

int IPC_captureMutex(const  IPC_handle handle, int timeout)
{
    if(!handle)
        return IPC_invalidHandle;

    ipc_handle_t h = (ipc_handle_t)handle;

	int status = WaitForSingleObject(h->ipc_descr, timeout);
	if(status == WAIT_TIMEOUT) 
		return IPC_generalError;

    return IPC_ok;
}

//-----------------------------------------------------------------------------

int IPC_releaseMutex(const  IPC_handle handle)
{
    if(!handle)
        return IPC_invalidHandle;

    ipc_handle_t h = (ipc_handle_t)handle;

	ReleaseMutex(h->ipc_descr);
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

	CloseHandle(h->ipc_descr);
	
    delete_ipc_object(h);
    return IPC_ok;
}

//-----------------------------------------------------------------------------

#endif //__IPC_WIN__

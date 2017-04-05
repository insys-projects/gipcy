
#ifdef __IPC_WIN__
#ifndef __WINIPC_H__
    #include "winipc.h"
#endif
#ifndef __MUTEXIPC_H__
    #include "mutexipc.h"
#endif

//-----------------------------------------------------------------------------

GIPCY_API IPC_handle IPC_createMutex(const IPC_str *name, int value)
{
    ipc_handle_t h = allocate_ipc_object(name, IPC_typeMutex);
    if(!h)
        return NULL;

    h->ipc_data = NULL;

    h->ipc_descr = CreateMutex(NULL, value, name);
    if(h->ipc_descr == NULL)
	{
        delete_ipc_object(h);
        return NULL;
    }

    return h;
}

//-----------------------------------------------------------------------------

GIPCY_API int IPC_captureMutex(const  IPC_handle handle, int timeout)
{
    if(!handle)
        return IPC_INVALID_HANDLE;

    ipc_handle_t h = (ipc_handle_t)handle;

	int status = WaitForSingleObject(h->ipc_descr, timeout);
	if(status == WAIT_TIMEOUT) 
		return IPC_WAIT_TIMEOUT;
	if(status == WAIT_ABANDONED) 
		return IPC_WAIT_ABANDONED;

    return IPC_OK;
}

//-----------------------------------------------------------------------------

GIPCY_API int IPC_releaseMutex(const  IPC_handle handle)
{
    if(!handle)
        return IPC_INVALID_HANDLE;

    ipc_handle_t h = (ipc_handle_t)handle;

	int ret = ReleaseMutex(h->ipc_descr);
	if(!ret)
	    return IPC_GENERAL_ERROR;
    return IPC_OK;
}

//-----------------------------------------------------------------------------

GIPCY_API int IPC_deleteMutex(IPC_handle handle)
{
    if(!handle)
        return IPC_INVALID_HANDLE;

    ipc_handle_t h = (ipc_handle_t)handle;

    if(h->ipc_type != IPC_typeMutex)
        return IPC_INVALID_HANDLE;

	int ret = CloseHandle(h->ipc_descr);
	if(!ret)
	    return IPC_GENERAL_ERROR;
	
    delete_ipc_object(h);
    return IPC_OK;
}

//-----------------------------------------------------------------------------

#endif //__IPC_WIN__

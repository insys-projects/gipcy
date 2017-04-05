
#ifdef __IPC_WIN__
#ifndef __WINIPC_H__
    #include "winipc.h"
#endif
#ifndef __EVENTIPC_H__
    #include "eventipc.h"
#endif

//-----------------------------------------------------------------------------

//	manual = TRUE - manual reset, FALSE - autoreset
//  value = TRUE - начальное состояние Signaled
GIPCY_API IPC_handle IPC_createEvent(const IPC_str *name, int manual, int value)
{
    ipc_handle_t h = allocate_ipc_object(name, IPC_typeEvent);
    if(!h)
        return NULL;

    h->ipc_data = NULL;

    h->ipc_descr = CreateEvent(NULL, manual, value, name);
    if(h->ipc_descr == NULL)
	{
        delete_ipc_object(h);
        return NULL;
    }

    return h;
}

//-----------------------------------------------------------------------------

GIPCY_API int IPC_waitEvent(const  IPC_handle handle, int timeout)
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

GIPCY_API int IPC_setEvent(const  IPC_handle handle)
{
    if(!handle)
        return IPC_INVALID_HANDLE;

    ipc_handle_t h = (ipc_handle_t)handle;

	int ret = SetEvent(h->ipc_descr);
	if(!ret)
	    return IPC_GENERAL_ERROR;
	return IPC_OK;
}

//-----------------------------------------------------------------------------

GIPCY_API int IPC_resetEvent(const  IPC_handle handle)
{
    if(!handle)
        return IPC_INVALID_HANDLE;

    ipc_handle_t h = (ipc_handle_t)handle;

	int ret = ResetEvent(h->ipc_descr);
	if(!ret)
	    return IPC_GENERAL_ERROR;
    return IPC_OK;
}

//-----------------------------------------------------------------------------

GIPCY_API int IPC_deleteEvent(IPC_handle handle)
{
    if(!handle)
        return IPC_INVALID_HANDLE;

    ipc_handle_t h = (ipc_handle_t)handle;

    if(h->ipc_type != IPC_typeEvent)
        return IPC_INVALID_HANDLE;

	int ret = CloseHandle(h->ipc_descr);
	if(!ret)
	    return IPC_GENERAL_ERROR;
	
    delete_ipc_object(h);
    return IPC_OK;
}

//-----------------------------------------------------------------------------

#endif //__IPC_WIN__

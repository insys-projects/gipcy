
#ifdef __IPC_WIN__
#ifndef __WINIPC_H__
    #include "winipc.h"
#endif
#ifndef __EVENTIPC_H__
    #include "eventipc.h"
#endif

//-----------------------------------------------------------------------------
#ifdef _WIN64
IPC_handle IPC_createEvent(const wchar_t *name, bool manual, bool value)
{
    ipc_handle_t h = allocate_ipc_object(name, IPC_typeEvent);
    if(!h)
        return NULL;

    h->ipc_data = NULL;

	h->ipc_descr = CreateEvent(NULL, manual, value, name);
    if(h->ipc_descr == INVALID_HANDLE_VALUE)
	{
        delete_ipc_object(h);
        return NULL;
    }

    return h;
}
#else
//	manual = TRUE - manual reset, FALSE - autoreset
//  value = TRUE - начальное состояние Signaled
IPC_handle IPC_createEvent(const char *name, bool manual, bool value)
{
    ipc_handle_t h = allocate_ipc_object(name, IPC_typeEvent);
    if(!h)
        return NULL;

    h->ipc_data = NULL;

    h->ipc_descr = CreateEvent(NULL, manual, value, name);
    if(h->ipc_descr == INVALID_HANDLE_VALUE)
	{
        delete_ipc_object(h);
        return NULL;
    }

    return h;
}
#endif

//-----------------------------------------------------------------------------

int IPC_waitEvent(const  IPC_handle handle, int timeout)
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

int IPC_setEvent(const  IPC_handle handle)
{
    if(!handle)
        return IPC_invalidHandle;

    ipc_handle_t h = (ipc_handle_t)handle;

	SetEvent(h->ipc_descr);
	return IPC_ok;
}

//-----------------------------------------------------------------------------

int IPC_resetEvent(const  IPC_handle handle, int value)
{
    if(!handle)
        return IPC_invalidHandle;

    ipc_handle_t h = (ipc_handle_t)handle;

	ResetEvent(h->ipc_descr);
    return IPC_ok;
}

//-----------------------------------------------------------------------------

int IPC_deleteEvent(IPC_handle handle)
{
    if(!handle)
        return IPC_invalidHandle;

    ipc_handle_t h = (ipc_handle_t)handle;

    if(h->ipc_type != IPC_typeEvent)
        return IPC_invalidHandle;

	CloseHandle(h->ipc_descr);
	
    delete_ipc_object(h);
    return IPC_ok;
}

//-----------------------------------------------------------------------------

#endif //__IPC_WIN__


#ifdef __IPC_WIN__

#ifndef __WINIPC_H__
    #include "winipc.h"
#endif
#ifndef __DLLIPC_H__
    #include "dllipc.h"
#endif

IPC_handle IPC_openLibrary(const IPC_str *name, unsigned param)
{
    ipc_handle_t h = allocate_ipc_object(name, IPC_typeLibrary);
    if(!h)
        return NULL;

    //! TODO insert code here
	h->ipc_descr = LoadLibrary(name);

    return h;
}

void* IPC_getEntry(IPC_handle handle, const char *entryName)
{
    if(!handle)
        return (void*)IPC_invalidHandle;
	ipc_handle_t h = (ipc_handle_t)handle;
	
	FARPROC pDlgFunc = GetProcAddress((HINSTANCE)(h->ipc_descr), entryName);
    
	return pDlgFunc;
}

int IPC_closeLibrary(IPC_handle handle)
{
    if(!handle)
        return IPC_invalidHandle;
	ipc_handle_t h = (ipc_handle_t)handle;

	BOOL ret = FreeLibrary((HINSTANCE)(h->ipc_descr));

    delete_ipc_object(h);
    return IPC_ok;
}

#endif //__IPC_WIN__

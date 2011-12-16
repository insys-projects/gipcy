
#ifdef __IPC_WIN__

#ifndef __WINIPC_H__
    #include "winipc.h"
#endif
#ifndef __SHMIPC_H__
    #include "shmipc.h"
#endif

//-----------------------------------------------------------------------------
IPC_handle IPC_createSharedMemory(const IPC_str *name, int size)
{
    ipc_handle_t h = allocate_ipc_object(name, IPC_typeSharedMem);
    if(!h)
        return NULL;

	h->ipc_size = size;

	h->ipc_descr = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, name);

    return h;
}

//-----------------------------------------------------------------------------
IPC_handle IPC_createSharedMemoryEx(const IPC_str *name, int size, int *alreadyCreated)
{
    ipc_handle_t h = allocate_ipc_object(name, IPC_typeSharedMem);
    if(!h)
        return NULL;

    h->ipc_size = size;

	h->ipc_descr = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, name);
	*alreadyCreated = ( GetLastError() == ERROR_ALREADY_EXISTS ) ? 1 : 0;

    return h;
}

//-----------------------------------------------------------------------------
void* IPC_mapSharedMemory(const  IPC_handle handle)
{
    if(!handle)
        return (void*)IPC_invalidHandle;
	ipc_handle_t h = (ipc_handle_t)handle;

	void* pMem = (PVOID)MapViewOfFile(h->ipc_descr, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);

	h->ipc_data = pMem;

    return pMem;
}

//-----------------------------------------------------------------------------
int IPC_unmapSharedMemory(const  IPC_handle handle)
{
    if(!handle)
        return IPC_invalidHandle;
	ipc_handle_t h = (ipc_handle_t)handle;

	UnmapViewOfFile(h->ipc_data);

	h->ipc_data = NULL;

    return IPC_ok;
}

//-----------------------------------------------------------------------------
int IPC_deleteSharedMemory(IPC_handle handle)
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

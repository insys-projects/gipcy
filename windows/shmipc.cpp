
#ifdef __IPC_WIN__

#ifndef __WINIPC_H__
    #include "winipc.h"
#endif
#ifndef __SHMIPC_H__
    #include "shmipc.h"
#endif

//-----------------------------------------------------------------------------
GIPCY_API IPC_handle IPC_createSharedMemory(const IPC_str *name, int size)
{
    ipc_handle_t h = allocate_ipc_object(name, IPC_typeSharedMem);
    if(!h)
        return NULL;

	h->ipc_size = size;

	h->ipc_descr = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, name);
	if(h->ipc_descr == NULL)
	{
	    delete_ipc_object(h);
		return NULL;
	}

    return h;
}

//-----------------------------------------------------------------------------
GIPCY_API IPC_handle IPC_createSharedMemoryEx(const IPC_str *name, int size, int *alreadyCreated)
{
    ipc_handle_t h = allocate_ipc_object(name, IPC_typeSharedMem);
    if(!h)
        return NULL;

    h->ipc_size = size;

	h->ipc_descr = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, name);
	if(h->ipc_descr == NULL)
	{
	    delete_ipc_object(h);
		return NULL;
	}
	*alreadyCreated = ( GetLastError() == ERROR_ALREADY_EXISTS ) ? 1 : 0;

    return h;
}

//-----------------------------------------------------------------------------
GIPCY_API IPC_handle IPC_openSharedMemory(const IPC_str *name)
{
	ipc_handle_t h = allocate_ipc_object(name, IPC_typeSharedMem);
	if(!h)
		return NULL;

	h->ipc_size = 0;

	h->ipc_descr = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, name);
	if(h->ipc_descr == NULL)
	{
		delete_ipc_object(h);
		return NULL;
	}

	return h;
}

//-----------------------------------------------------------------------------
GIPCY_API void* IPC_mapSharedMemory(const  IPC_handle handle)
{
    if(!handle)
        return NULL;
	ipc_handle_t h = (ipc_handle_t)handle;

	void* pMem = (PVOID)MapViewOfFile(h->ipc_descr, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	h->ipc_data = pMem;
	if(h->ipc_data == NULL)
		return NULL;

    return pMem;
}

//-----------------------------------------------------------------------------
GIPCY_API int IPC_unmapSharedMemory(const  IPC_handle handle)
{
    if(!handle)
        return IPC_INVALID_HANDLE;
	ipc_handle_t h = (ipc_handle_t)handle;
	if(!h->ipc_data) 
		return IPC_GENERAL_ERROR;

	int ret = UnmapViewOfFile(h->ipc_data);
	if(!ret)
	    return IPC_GENERAL_ERROR;

	h->ipc_data = NULL;

    return IPC_OK;
}

//-----------------------------------------------------------------------------
GIPCY_API int IPC_deleteSharedMemory(IPC_handle handle)
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

#endif //__IPC_WIN__

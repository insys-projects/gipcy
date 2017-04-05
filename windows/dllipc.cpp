
#ifdef __IPC_WIN__

#ifndef __WINIPC_H__
    #include "winipc.h"
#endif
#ifndef __DLLIPC_H__
    #include "dllipc.h"
#endif

#ifdef _WIN64
GIPCY_API IPC_handle IPC_openLibraryEx(const IPC_str *baseName, unsigned param)
{
    ipc_handle_t h = allocate_ipc_object(baseName, IPC_typeLibrary);
    if(!h)
        return NULL;

	IPC_str libname[MAX_PATH];
	IPC_str libname0[MAX_PATH];
	wcscpy(libname, baseName);
	wchar_t* pchar = wcsstr(libname, L".dll");
	if(pchar != NULL)
		*pchar = 0;
	size_t len_name =  wcslen(libname);
	wcscpy(libname0, libname);
	if(libname[len_name-2] != '6' && libname[len_name-1] != '4')
	{
		wcscat(libname, L"64");
		wcscat(libname0, L"-64");
	}
	h->ipc_descr = LoadLibrary(libname);
    if( !h->ipc_descr )
	{
		h->ipc_descr = LoadLibrary(libname0);
		if(h->ipc_descr == NULL)
		{
			delete_ipc_object(h);
			return NULL;
		}
	}
    return h;
}
#else
GIPCY_API	IPC_handle IPC_openLibraryEx(const IPC_str *baseName, unsigned param)
{
    ipc_handle_t h = allocate_ipc_object(baseName, IPC_typeLibrary);
    if(!h)
        return NULL;

    //! TODO insert code here
	h->ipc_descr = LoadLibrary(baseName);
    if(h->ipc_descr == NULL)
	{
        delete_ipc_object(h);
        return NULL;
    }

    return h;
}
#endif

GIPCY_API	IPC_handle IPC_openLibrary(const IPC_str *name, unsigned param)
{
    ipc_handle_t h = allocate_ipc_object(name, IPC_typeLibrary);
    if(!h)
        return NULL;

    //! TODO insert code here
	h->ipc_descr = LoadLibrary(name);
    if(h->ipc_descr == NULL)
	{
        delete_ipc_object(h);
        return NULL;
    }

    return h;
}

GIPCY_API void* IPC_getEntry(IPC_handle handle, const char *entryName)
{
    if(!handle)
        return NULL;
	ipc_handle_t h = (ipc_handle_t)handle;
	
	FARPROC pDlgFunc = GetProcAddress((HINSTANCE)(h->ipc_descr), entryName);
    
	return pDlgFunc;
}

GIPCY_API int IPC_closeLibrary(IPC_handle handle)
{
    if(!handle)
        return IPC_INVALID_HANDLE;
	ipc_handle_t h = (ipc_handle_t)handle;

	int ret = FreeLibrary((HINSTANCE)(h->ipc_descr));
	if(!ret)
	    return IPC_GENERAL_ERROR;

    delete_ipc_object(h);
    return IPC_OK;
}

#endif //__IPC_WIN__


#ifdef __IPC_WIN__

#ifndef __WINIPC_H__
    #include "winipc.h"
#endif

#ifndef __IPCLIB_H__
    #include "ipclib.h"
#endif
//-----------------------------------------------------------------------------
#ifdef _UNICODE
ipc_handle_t allocate_ipc_object(const wchar_t *name, IPC_type type)
{
    ipc_handle_t h = (ipc_handle_t)malloc(sizeof(struct ipc_t));
    if(!h) {
        return NULL;
    }

    memset(h,0,sizeof(struct ipc_t));

	int sizeName = (int)wcslen(name)+1;

	sizeName *= 2;

    h->ipc_name = (wchar_t*)malloc(sizeName);
    if(!h->ipc_name) {
        free(h);
        return NULL;
    }

    memset(h->ipc_name,0,sizeName);
    memcpy(h->ipc_name,name,sizeName);

	h->ipc_type = type;

    return h;
}
#else
ipc_handle_t allocate_ipc_object(const char *name, IPC_type type)
{
    ipc_handle_t h = (ipc_handle_t)malloc(sizeof(struct ipc_t));
    if(!h) {
        return NULL;
    }

    memset(h,0,sizeof(struct ipc_t));

    int sizeName = (int)strlen(name)+1;

    h->ipc_name = (char*)malloc(sizeName);
    if(!h->ipc_name) {
        free(h);
        return NULL;
    }

    memset(h->ipc_name,0,sizeName);
    memcpy(h->ipc_name,name,strlen(name));

	h->ipc_type = type;

    return h;
}
#endif

//-----------------------------------------------------------------------------

void delete_ipc_object(ipc_handle_t h)
{
    if(!h) return;

    //! сделать что-нить зависящее от типа IPC
    switch(h->ipc_type) {
    case IPC_typeSemaphore:
        break;
    case IPC_typeSharedMem:
        break;
    case IPC_typeMutex:
        break;
    case IPC_typeEvent:
        break;
    case IPC_typeThread:
        break;
    }

    if(h->ipc_name)
        free(h->ipc_name);

    free(h);
}

//-----------------------------------------------------------------------------

int IPC_init(void)
{
	return 0;
}

//-----------------------------------------------------------------------------

int IPC_cleanup(void)
{
	return 0;
}

//-----------------------------------------------------------------------------

size_t IPC_getDescriptor(IPC_handle handle)
{
    ipc_handle_t h = (ipc_handle_t)handle;
	return (size_t)h->ipc_descr;
}

//-----------------------------------------------------------------------------

int IPC_closeHandle(IPC_handle handle)
{
    delete_ipc_object((ipc_handle_t)handle);

    return IPC_OK;
}

//-----------------------------------------------------------------------------

#endif //__IPC_WIN__

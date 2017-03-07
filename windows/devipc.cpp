
#ifdef __IPC_WIN__

#ifndef __WINIPC_H__
    #include "winipc.h"
#endif
#ifndef __DEVIPC_H__
    #include "devipc.h"
#endif

//-----------------------------------------------------------------------------
// открыть устройство
#ifdef _UNICODE
GIPCY_API IPC_handle IPC_openDevice(wchar_t *devname, const wchar_t *mainname, int devnum)
{
    if(!mainname) return NULL;

	swprintf(devname, L"\\\\.\\%s%d", mainname, devnum);
    ipc_handle_t h = allocate_ipc_object(devname, IPC_typeFile);
    if(!h) return NULL;

    h->ipc_size = 0;

	h->ipc_descr = CreateFile(devname,
							GENERIC_READ | GENERIC_WRITE,
							FILE_SHARE_READ | FILE_SHARE_WRITE,
							NULL, 
							OPEN_EXISTING, 
							FILE_FLAG_OVERLAPPED, // 0,
							NULL);
	if(h->ipc_descr == INVALID_HANDLE_VALUE)
	{
		delete_ipc_object(h);
	   	return NULL;
	}
	return h;
}
#else
GIPCY_API IPC_handle IPC_openDevice(char *devname, const char *mainname, int devnum)
{
    if(!mainname) return NULL;

	sprintf(devname, "\\\\.\\%s%d", mainname, devnum);
	ipc_handle_t h = allocate_ipc_object(devname, IPC_typeFile);
    if(!h) return NULL;

    h->ipc_size = 0;

	h->ipc_descr = CreateFile(devname,
							GENERIC_READ | GENERIC_WRITE,
							FILE_SHARE_READ | FILE_SHARE_WRITE,
							NULL, 
							OPEN_EXISTING, 
							FILE_FLAG_OVERLAPPED, // 0,
							NULL);
	if(h->ipc_descr == INVALID_HANDLE_VALUE)
	{
		delete_ipc_object(h);
	   	return NULL;
	}
	return h;
}
#endif

//-----------------------------------------------------------------------------

GIPCY_API int IPC_closeDevice(IPC_handle handle)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_INVALID_HANDLE;

    int ret = CloseHandle(h->ipc_descr);
	if(!ret)
	    return IPC_GENERAL_ERROR;
    delete_ipc_object(h);
    return IPC_OK;
}

//-----------------------------------------------------------------------------

GIPCY_API int IPC_readDevice(IPC_handle handle, void *data, int size)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_INVALID_HANDLE;

	unsigned long readsize;
    int ret = ReadFile(h->ipc_descr, data, size, &readsize, NULL);
	if(!ret)
	    return IPC_GENERAL_ERROR;

    return IPC_OK;
}

//-----------------------------------------------------------------------------

GIPCY_API int IPC_writeDevice(IPC_handle handle, void *data, int size)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_INVALID_HANDLE;

	unsigned long writesize;
    int ret = WriteFile(h->ipc_descr, data, size, &writesize, NULL);
	if(!ret)
	    return IPC_GENERAL_ERROR;

    return IPC_OK;
}

//-----------------------------------------------------------------------------

GIPCY_API int IPC_ioctlDevice(IPC_handle handle, unsigned long cmd, void *srcBuf, int srcSize, void *dstBuf, int dstSize)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_INVALID_HANDLE;

    //int res = ioctl(h->ipc_descr, cmd, param);

	ULONG   length;     // the return length from the driver
	int res = DeviceIoControl(h->ipc_descr,
								cmd,
								srcBuf,
								srcSize,
								dstBuf,
								dstSize,
								&length,
								NULL);

	if(!res)
	    return IPC_GENERAL_ERROR;
    return IPC_OK;
}

GIPCY_API int IPC_ioctlDeviceOvl(IPC_handle handle, unsigned long cmd, void *srcBuf, int srcSize, void *dstBuf, int dstSize, void *overlap)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_INVALID_HANDLE;

    //int res = ioctl(h->ipc_descr, cmd, param);

	ULONG   length;     // the return length from the driver
	int res = DeviceIoControl(h->ipc_descr,
								cmd,
								srcBuf,
								srcSize,
								dstBuf,
								dstSize,
								&length,
								(LPOVERLAPPED)overlap);

	if(!res)
	    return IPC_GENERAL_ERROR;
    return IPC_OK;
}

//-----------------------------------------------------------------------------
GIPCY_API int IPC_mapPhysAddr(IPC_handle handle, void** virtAddr, size_t physAddr, unsigned long size)
{
    return IPC_OK;
}

//-----------------------------------------------------------------------------
GIPCY_API int IPC_unmapPhysAddr(IPC_handle handle, void* virtAddr, unsigned long size)
{
    return IPC_OK;
}

//-----------------------------------------------------------------------------

#endif //__IPC_WIN__


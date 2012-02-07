
#ifdef __IPC_WIN__

#include <stdio.h>

#ifndef __WINIPC_H__
    #include "winipc.h"
#endif
#ifndef __SYSIPC_H__
    #include "sysipc.h"
#endif

//-----------------------------------------------------------------------------
// выполнить задержку при выполнении программы
GIPCY_API void IPC_delay(int ms)
{
    Sleep(ms);
}

//-----------------------------------------------------------------------------
// получить последнюю ошибку системной функции
GIPCY_API int IPC_sysError()
{
	return GetLastError();
}

//-----------------------------------------------------------------------------
GIPCY_API int IPC_getFullPath(const IPC_str *name, IPC_str *path)
{
	IPC_str *FirsChar;
	ULONG res = GetFullPathName(name, MAX_PATH, path, &FirsChar);
	if(!res)
		return IPC_GENERAL_ERROR;
    return IPC_OK;
}

//-----------------------------------------------------------------------------
GIPCY_API const IPC_str* IPC_getCurrentDir(IPC_str *buf, int size)
{
	GetCurrentDirectory(size, buf);
	return buf;

}

//-----------------------------------------------------------------------------
// открыть устройство
#ifdef _WIN64
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
	return h;
}
#endif

//-----------------------------------------------------------------------------

GIPCY_API int IPC_closeDevice(IPC_handle handle)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_INVALID_HANDLE;

    int res = CloseHandle(h->ipc_descr);
    delete_ipc_object(h);
    return res;
}

//-----------------------------------------------------------------------------

GIPCY_API int IPC_readDevice(IPC_handle handle, void *data, int size)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_INVALID_HANDLE;

	unsigned long readsize;
    int res = ReadFile(h->ipc_descr, data, size, &readsize, NULL);

    return res;
}

//-----------------------------------------------------------------------------

GIPCY_API int IPC_writeDevice(IPC_handle handle, void *data, int size)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_INVALID_HANDLE;

	unsigned long writesize;
    int res = WriteFile(h->ipc_descr, data, size, &writesize, NULL);

    return res;
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

    return res;
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

    return res;
}


// открыть файл
// amode (access_mode) = GENERIC_READ, GENERIC_WRITE
// smode (share_mode) = 0 (запрет), FILE_SHARE_READ, FILE_SHARE_WRITE
//                   можно всегда устанавливать FILE_SHARE_WRITE | FILE_SHARE_READ (_SH_DENYNO)
// cmode (exist_mode) = CREATE_ALWAYS, OPEN_EXISTING
// flag = FILE_ATTRIBUTE_NORMAL, FILE_FLAG_NO_BUFFERING
GIPCY_API IPC_handle IPC_openFile(const IPC_str *name, int flags)
{
    if(!name) return NULL;
	ipc_handle_t h = allocate_ipc_object(name, IPC_typeFile);
    if(!h) return NULL;

    h->ipc_size = 0;

	unsigned long amode = 0;
	unsigned long cmode = 0;
	if(IPC_CREATE_FILE & (flags & 0xf))
		cmode = CREATE_ALWAYS;
	if(IPC_OPEN_FILE & (flags & 0xf))
		cmode = OPEN_EXISTING;

	if(IPC_FILE_RDONLY & (flags & 0xf0))
		amode = GENERIC_READ;
	if(IPC_FILE_WRONLY & (flags & 0xf0))
		amode |= GENERIC_WRITE;
	if(IPC_FILE_RDWR & (flags & 0xf0))
		amode = GENERIC_READ | GENERIC_WRITE;

	h->ipc_descr = CreateFile(name,
							  amode,
							  FILE_SHARE_WRITE | FILE_SHARE_READ,
							  NULL,
							  cmode,
							  FILE_ATTRIBUTE_NORMAL,
							  NULL);
	if(h->ipc_descr == INVALID_HANDLE_VALUE)
	   	return NULL;
   	return h;
}

GIPCY_API IPC_handle IPC_openFileEx(const IPC_str *name, int flags, int attr)
{
    if(!name) return NULL;
	ipc_handle_t h = allocate_ipc_object(name, IPC_typeFile);
    if(!h) return NULL;

    h->ipc_size = 0;

	unsigned long amode = 0;
	unsigned long cmode = 0;
	unsigned long fattr = 0;
	if(IPC_CREATE_FILE & (flags & 0xf))
		cmode = CREATE_ALWAYS;
	if(IPC_OPEN_FILE & (flags & 0xf))
		cmode = OPEN_EXISTING;

	if(IPC_FILE_RDONLY & (flags & 0xf0))
		amode = GENERIC_READ;
	if(IPC_FILE_WRONLY & (flags & 0xf0))
		amode |= GENERIC_WRITE;
	if(IPC_FILE_RDWR & (flags & 0xf0))
		amode = GENERIC_READ | GENERIC_WRITE;

	if(attr == IPC_FILE_NORMAL)
		fattr = FILE_ATTRIBUTE_NORMAL;
	if(attr == IPC_FILE_NOBUFFER)
		fattr = FILE_FLAG_NO_BUFFERING;
	if(attr == IPC_FILE_WRTHROUGH)
		fattr = FILE_FLAG_WRITE_THROUGH;

	h->ipc_descr = CreateFile(name,
							  amode,
							  FILE_SHARE_WRITE | FILE_SHARE_READ,
							  NULL,
							  cmode,
							  fattr,
							  NULL);
	if(h->ipc_descr == INVALID_HANDLE_VALUE)
	   	return NULL;
   	return h;
}
/*
	// открытие файла для прямой записи на диск
	HANDLE hfile = CreateFile(	fileName,
								GENERIC_WRITE,
//								FILE_SHARE_WRITE | FILE_SHARE_READ,
								0,
								NULL,
								CREATE_ALWAYS,
//								FILE_ATTRIBUTE_NORMAL,
							    FILE_FLAG_NO_BUFFERING,// | FILE_FLAG_WRITE_THROUGH,         
								NULL);
	if(hfile == INVALID_HANDLE_VALUE)
		
	// открытие файла для записи собранных данных
	HANDLE hfile = CreateFile(	fileName,
								GENERIC_WRITE,
								FILE_SHARE_WRITE | FILE_SHARE_READ,
								NULL,
								CREATE_ALWAYS,
								FILE_ATTRIBUTE_NORMAL,
								NULL);
	if(hfile == INVALID_HANDLE_VALUE)

	// открытие файла для дозаписи собранных данных в конец файла
		hfile = CreateFile(	fileName,
								GENERIC_WRITE,
								FILE_SHARE_WRITE | FILE_SHARE_READ,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,
								NULL);
		LONG HiPart = 0;
		SetFilePointer(hfile, 0, &HiPart, FILE_END);

	// дозапись хвоста в конец файла с данными
	HANDLE hfile = CreateFile(	fileName,
								GENERIC_READ | GENERIC_WRITE,
								FILE_SHARE_WRITE | FILE_SHARE_READ,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,
								NULL);
	if(hfile == INVALID_HANDLE_VALUE)
	{
		BRDC_printf(_BRDC("Create file %s error.\n"), fileName);
		return;
	}
	long lDistanceToMoveLow = 0L;
	long lDistanceToMoveHigh = 0L;
	ULONG BytesNum = SetFilePointer(hfile, lDistanceToMoveLow, &lDistanceToMoveHigh, FILE_END);

////////////////////////////////////////////
	// эти функции есть в LINUX, они входят в стандарт языка
	// fopen(), fclose(), fputs(), fgets()
	// открытие лог-файла в brdshell
	flog = BRDC_fopen( logFile, _BRDC("wt") );	
	// открытие ini-файла в brdshell
	fin = BRDC_fopen( REG_FILENAME, _BRDC("rt") );
////////////////////////////////////////////

	// запись флагового файла
	fs = BRDC_sopen(fileName, _O_WRONLY|_O_BINARY|_O_CREAT|_O_TRUNC, _SH_DENYNO, _S_IWRITE);
	// чтение флагового файла
    fs = BRDC_sopen(fileName, _O_RDONLY|_O_BINARY, _SH_DENYNO, _S_IREAD);

}
*/
//-----------------------------------------------------------------------------

GIPCY_API int IPC_closeFile(IPC_handle handle)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_INVALID_HANDLE;

    int res = CloseHandle(h->ipc_descr);
    delete_ipc_object(h);
    return res;
}

//-----------------------------------------------------------------------------

GIPCY_API int IPC_readFile(IPC_handle handle, void *data, int size)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_INVALID_HANDLE;

	unsigned long readsize;
    int res = ReadFile(h->ipc_descr, data, size, &readsize, NULL);
	if(res == TRUE)
	    return IPC_OK;
	else
		return 1;
}

//-----------------------------------------------------------------------------

GIPCY_API int IPC_writeFile(IPC_handle handle, void *data, int size)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_INVALID_HANDLE;

	unsigned long writesize;
    int res = WriteFile(h->ipc_descr, data, size, &writesize, NULL);
	if(res == TRUE)
	    return IPC_OK;
	else
		return 1;

    return res;
}

//-----------------------------------------------------------------------------

GIPCY_API int IPC_setPosFile(IPC_handle handle, int pos, int method)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_INVALID_HANDLE;

	LONG HiPart = 0;
	int res = SetFilePointer(h->ipc_descr, pos, &HiPart, method);
	if(HiPart)
		res = -1;

    return res;
}

//-----------------------------------------------------------------------------

GIPCY_API int IPC_getPrivateProfileString(const IPC_str *lpAppName, const IPC_str *lpKeyName, const IPC_str *lpDefault,
                                IPC_str *lpReturnedString, int nSize, const IPC_str *lpFileName)
{
    return GetPrivateProfileString(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName);
}

//-----------------------------------------------------------------------------

GIPCY_API int IPC_writePrivateProfileString(const IPC_str *lpAppName, const IPC_str *lpKeyName, const IPC_str *lpString, const IPC_str *lpFileName)
{
    return WritePrivateProfileString( lpAppName, lpKeyName, lpString, lpFileName );
}

//-----------------------------------------------------------------------------

GIPCY_API long IPC_interlockedDecrement(volatile long *val )
{
    return InterlockedDecrement(val);
}

//-----------------------------------------------------------------------------

GIPCY_API long IPC_interlockedIncrement(volatile long *val )
{
    return InterlockedIncrement(val);
}

//-----------------------------------------------------------------------------

GIPCY_API long IPC_interlockedCompareExchange(volatile long *dst, long val, long param )
{
    return InterlockedCompareExchange(dst, val, param);
}

//-----------------------------------------------------------------------------

GIPCY_API long IPC_interlockedExchange(volatile long *dst, long val )
{
    return InterlockedExchange(dst, val);
}

//-----------------------------------------------------------------------------

GIPCY_API long IPC_interlockedExchangeAdd(volatile long *dst, long val )
{
    return InterlockedExchangeAdd(dst, val);
}

//-----------------------------------------------------------------------------

GIPCY_API IPC_tls_key IPC_createTlsKey(void)
{
    return TlsAlloc();
}

//-----------------------------------------------------------------------------

GIPCY_API void* IPC_tlsGetValue(IPC_tls_key key)
{
    return TlsGetValue(key);
}

//-----------------------------------------------------------------------------

GIPCY_API int IPC_tlsSetValue(IPC_tls_key key, void *ptr)
{
    return TlsSetValue(key, ptr);
}

//-----------------------------------------------------------------------------

GIPCY_API int IPC_deleteTlsKey(IPC_tls_key key)
{
     return TlsFree(key);
}

//-----------------------------------------------------------------------------

#endif //__IPC_WIN__


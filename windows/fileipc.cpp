
#ifdef __IPC_WIN__

#ifndef __WINIPC_H__
    #include "winipc.h"
#endif
#ifndef __FILEIPC_H__
    #include "fileipc.h"
#endif

//-----------------------------------------------------------------------------
// ������� ����
// amode (access_mode) = GENERIC_READ, GENERIC_WRITE
// smode (share_mode) = 0 (������), FILE_SHARE_READ, FILE_SHARE_WRITE
//                   ����� ������ ������������� FILE_SHARE_WRITE | FILE_SHARE_READ (_SH_DENYNO)
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
	{
	    delete_ipc_object(h);
	   	return NULL;
	}
   	return h;
}

//-----------------------------------------------------------------------------
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
	{
	    delete_ipc_object(h);
	   	return NULL;
	}
   	return h;
}
/*
	// �������� ����� ��� ������ ������ �� ����
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
		
	// �������� ����� ��� ������ ��������� ������
	HANDLE hfile = CreateFile(	fileName,
								GENERIC_WRITE,
								FILE_SHARE_WRITE | FILE_SHARE_READ,
								NULL,
								CREATE_ALWAYS,
								FILE_ATTRIBUTE_NORMAL,
								NULL);
	if(hfile == INVALID_HANDLE_VALUE)

	// �������� ����� ��� �������� ��������� ������ � ����� �����
		hfile = CreateFile(	fileName,
								GENERIC_WRITE,
								FILE_SHARE_WRITE | FILE_SHARE_READ,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,
								NULL);
		LONG HiPart = 0;
		SetFilePointer(hfile, 0, &HiPart, FILE_END);

	// �������� ������ � ����� ����� � �������
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
	// ��� ������� ���� � LINUX, ��� ������ � �������� �����
	// fopen(), fclose(), fputs(), fgets()
	// �������� ���-����� � brdshell
	flog = BRDC_fopen( logFile, _BRDC("wt") );	
	// �������� ini-����� � brdshell
	fin = BRDC_fopen( REG_FILENAME, _BRDC("rt") );
////////////////////////////////////////////

	// ������ ��������� �����
	fs = BRDC_sopen(fileName, _O_WRONLY|_O_BINARY|_O_CREAT|_O_TRUNC, _SH_DENYNO, _S_IWRITE);
	// ������ ��������� �����
    fs = BRDC_sopen(fileName, _O_RDONLY|_O_BINARY, _SH_DENYNO, _S_IREAD);

}
*/
//-----------------------------------------------------------------------------

GIPCY_API int IPC_closeFile(IPC_handle handle)
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

GIPCY_API int IPC_readFile(IPC_handle handle, void *data, int size)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_INVALID_HANDLE;

	unsigned long readsize;
    int ret = ReadFile(h->ipc_descr, data, size, &readsize, NULL);
	if(!ret)
	    return IPC_GENERAL_ERROR;
    //return IPC_OK;
	return readsize;
}

//-----------------------------------------------------------------------------

GIPCY_API int IPC_writeFile(IPC_handle handle, void *data, int size)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_INVALID_HANDLE;

	unsigned long writesize;
    int ret = WriteFile(h->ipc_descr, data, size, &writesize, NULL);
	if(!ret)
	    return IPC_GENERAL_ERROR;
    //return IPC_OK;
	return writesize;
}

//-----------------------------------------------------------------------------

GIPCY_API int IPC_setPosFile(IPC_handle handle, int pos, int method)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_INVALID_HANDLE;

	LONG HiPart = 0;
	DWORD ret = SetFilePointer(h->ipc_descr, pos, &HiPart, method);
	if(ret == 0xFFFFFFFF)
	    return IPC_GENERAL_ERROR;
    return IPC_OK;
}

//-----------------------------------------------------------------------------

GIPCY_API int IPC_getFileSize(IPC_handle handle, long long* size)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_INVALID_HANDLE;

	LARGE_INTEGER file_size;

	int ret = GetFileSizeEx(h->ipc_descr, &file_size);
	if(!ret)
	    return IPC_GENERAL_ERROR;

    *size = (long long)file_size.QuadPart;

    return IPC_OK;
}

#endif //__IPC_WIN__



#ifdef __IPC_QNX__

#ifndef __QNXIPC_H__
    #include "qnxipc.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/ioctl.h>

//-----------------------------------------------------------------------------

static int convert_ipc_flags(int ipc_flags)
{
    int flags = 0;

    if(ipc_flags & IPC_CREATE_FILE) {
        flags |= (O_CREAT | O_TRUNC);
    }

    if(ipc_flags & IPC_OPEN_FILE) {
        flags |= (O_CREAT | O_RDWR);
    }

    if(ipc_flags & IPC_FILE_RDONLY) {
        flags |= (O_RDONLY);
    }

    if(ipc_flags & IPC_FILE_WRONLY) {
        flags |= (O_CREAT | O_WRONLY);
    }

    if(ipc_flags & IPC_FILE_RDWR) {
        flags |= (O_CREAT | O_RDWR);
    }

    return flags;
}

//-----------------------------------------------------------------------------

IPC_handle IPC_openFile(const IPC_str *name, int flags)
{
    DEBUG_PRINT("%s(%s)\n", __FUNCTION__, name );

    if(!name) return NULL;

    ipc_handle_t h = allocate_ipc_object(name, IPC_typeFile);
    if(!h) return NULL;

    h->ipc_size = 0;

    int sysflag = convert_ipc_flags(flags);

    h->ipc_descr.ipc_file = open(name, sysflag, 0666);
    if(h->ipc_descr.ipc_file < 0)
	{
        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
	    delete_ipc_object(h);
        return NULL;
    }

    DEBUG_PRINT("%s(): open file - %s\n", __FUNCTION__, name );

    return h;
}

//-----------------------------------------------------------------------------

IPC_handle IPC_openFileEx(const IPC_str *name, int flags, int attr)
{
    DEBUG_PRINT("%s(%s)\n", __FUNCTION__, name );

    if(!name) return NULL;

    ipc_handle_t h = allocate_ipc_object(name, IPC_typeFile);
    if(!h) return NULL;

    h->ipc_size = 0;

    int sysflag = convert_ipc_flags(flags);

    h->ipc_descr.ipc_file = open(name, sysflag, 0666);
    if(h->ipc_descr.ipc_file < 0)
	{
        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
	    delete_ipc_object(h);
        return NULL;
    }

    DEBUG_PRINT("%s(): open file - %s\n", __FUNCTION__, name );

    return h;
}

//-----------------------------------------------------------------------------

int IPC_closeFile(IPC_handle handle)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_INVALID_HANDLE;

    int res = close(h->ipc_descr.ipc_file);
    if(res < 0) {
            DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
            return IPC_GENERAL_ERROR;
    }

    DEBUG_PRINT("%s(): close file - %s\n", __FUNCTION__, h->ipc_name );

    delete_ipc_object(h);

    return IPC_OK;
}

//-----------------------------------------------------------------------------

int IPC_readFile(IPC_handle handle, void *data, int size)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_INVALID_HANDLE;

    int res = read(h->ipc_descr.ipc_file,data,size);
    if(res < 0) {
        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
        return IPC_GENERAL_ERROR;
    }
    //return IPC_OK;
	return res;
}

//-----------------------------------------------------------------------------

int IPC_writeFile(IPC_handle handle, void *data, int size)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_INVALID_HANDLE;

    int res = write(h->ipc_descr.ipc_file,data,size);
    if(res < 0) {
        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
        return IPC_GENERAL_ERROR;
    }
    //return IPC_OK;
	return res;
}

//-----------------------------------------------------------------------------

int IPC_setPosFile(IPC_handle handle, int pos, int method)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_INVALID_HANDLE;

    int res = lseek(h->ipc_descr.ipc_file,pos,method);
    if(res <= 0) {
        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
        return IPC_GENERAL_ERROR;
    }

    return IPC_OK;
}

//-----------------------------------------------------------------------------

int IPC_getFileSize(IPC_handle handle, long long* size)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_INVALID_HANDLE;

	struct stat finfo;

    int res = fstat(h->ipc_descr.ipc_file, &finfo );
	if(res < 0) {
        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
        return IPC_GENERAL_ERROR;
    }
    *size = (long long)finfo.st_size;
    return IPC_OK;
}

//-----------------------------------------------------------------------------

#endif //__IPC_QNX__

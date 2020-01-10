
#ifdef __IPC_LINUX__

#include "linipc.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <error.h>
#include <time.h>
#include <sys/ioctl.h>

//-----------------------------------------------------------------------------

IPC_handle IPC_openDevice(IPC_str *devname, const IPC_str *mainname, int devnum)
{
    DEBUG_PRINT("%s(%s)\n", __FUNCTION__, mainname);

    if(!mainname || !devname) return NULL;

    snprintf( devname, 128, "%s%s%d", "/dev/", mainname, devnum);
    ipc_handle_t h = allocate_ipc_object(devname, IPC_typeFile);
    if(!h) return NULL;

    h->ipc_size = 0;

    h->ipc_descr.ipc_file = open(devname, O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH|S_IWOTH);
    if(h->ipc_descr.ipc_file < 0)
    {
        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
        delete_ipc_object(h);
        return NULL;
    }

    DEBUG_PRINT("%s(): open file - %s\n", __FUNCTION__, devname );

    return h;
}

//-----------------------------------------------------------------------------

IPC_handle IPC_openDeviceRaw(const IPC_str *devname)
{
    if(!devname) return NULL;

    ipc_handle_t h = allocate_ipc_object(devname, IPC_typeFile);
    if(!h) return NULL;

    h->ipc_size = 0;

    h->ipc_descr.ipc_file = open(devname, O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH|S_IWOTH);
    if(h->ipc_descr.ipc_file < 0)
    {
        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
        delete_ipc_object(h);
        return NULL;
    }

    DEBUG_PRINT("%s(): open file - %s\n", __FUNCTION__, devname );

    return h;
}

//-----------------------------------------------------------------------------

int IPC_closeDevice(IPC_handle handle)
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

int IPC_readDevice(IPC_handle handle, void *data, int size)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_INVALID_HANDLE;

    int res = read(h->ipc_descr.ipc_file,data,size);
    if(res < 0) {
        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
        return IPC_GENERAL_ERROR;
    }

    return IPC_OK;
}

//-----------------------------------------------------------------------------

int IPC_writeDevice(IPC_handle handle, void *data, int size)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_INVALID_HANDLE;

    int res = write(h->ipc_descr.ipc_file,data,size);
    if(res < 0) {
        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
        return IPC_GENERAL_ERROR;
    }

    return IPC_OK;
}

//-----------------------------------------------------------------------------

int IPC_ioctlDevice(IPC_handle handle, unsigned long cmd, void *srcBuf, int srcSize, void *dstBuf, int dstSize)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_INVALID_HANDLE;

    struct ioctl_param param;

    param.srcBuf = srcBuf;
    param.srcSize = srcSize;
    param.dstBuf = dstBuf;
    param.dstSize = dstSize;

    int res = ioctl(h->ipc_descr.ipc_file,cmd,&param);
    if(res < 0) {
        DEBUG_PRINT("%s(%d): %s\n", __FUNCTION__, int(cmd & 0xff), strerror(errno) );
        return IPC_GENERAL_ERROR;
    }

    return res;
}

//-----------------------------------------------------------------------------

int IPC_ioctlDeviceOvl(IPC_handle handle, unsigned long cmd, void *srcBuf, int srcSize, void *dstBuf, int dstSize, void *overlap)
{
    return IPC_ioctlDevice(handle, cmd, srcBuf, srcSize, dstBuf, dstSize);
}

//-----------------------------------------------------------------------------

GIPCY_API int IPC_mapPhysAddr(IPC_handle handle, void** virtAddr, size_t physAddr, unsigned long size)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_INVALID_HANDLE;

    void* vAddress = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, h->ipc_descr.ipc_file, (off_t)physAddr);
    if(vAddress == MAP_FAILED )
    {
        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
        virtAddr = NULL;
        return IPC_GENERAL_ERROR;
    }
    DEBUG_PRINT("%s(): Physical Address 0x%x -> Virtual Address %p\n", __FUNCTION__, physAddr, vAddress);
    *virtAddr = vAddress;

    // Debug On Zynq we can have only 16 shared memory objects per process !!!!
/*
    uint32_t *pmem = (uint32_t*)vAddress;
    uint32_t dummy_read = pmem[0];
    dummy_read = dummy_read;
    DEBUG_PRINT("%s(): Dummy read address - %p [0x%lx] - DONE\n", __FUNCTION__, vAddress, size);
    pmem[0] = dummy_read;
    DEBUG_PRINT("%s(): Dummy write address - %p [0x%lx] - DONE\n", __FUNCTION__, vAddress, size);
*/

    return IPC_OK;
}

//-----------------------------------------------------------------------------

GIPCY_API int IPC_unmapPhysAddr(IPC_handle handle, void* virtAddr, unsigned long size)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_INVALID_HANDLE;

    if(munmap( virtAddr, size ) < 0 )
    {
        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
        return IPC_GENERAL_ERROR;
    }

    return IPC_OK;
}

#endif //__IPC_LINUX__

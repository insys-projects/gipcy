
#ifdef __IPC_LINUX__

#ifndef __LINIPC_H__
    #include "linipc.h"
#endif
#ifndef __SYSIPC_H__
    #include "sysipc.h"
#endif

#include <errno.h>
#include <sys/ioctl.h>

//-----------------------------------------------------------------------------

void IPC_delay(int ms)
{
    struct timeval tv = {0, 0};
    tv.tv_usec = 1000*ms;

    select(0,NULL,NULL,NULL,&tv);
}

//-----------------------------------------------------------------------------

int IPC_sysError()
{
    return errno;
}

//-----------------------------------------------------------------------------

IPC_handle IPC_openFile(const IPC_str *name, int flags, int mode)
{
    DEBUG_PRINT("%s(%s)\n", __FUNCTION__, name );

    if(!name) return NULL;

    ipc_handle_t h = allocate_ipc_object(name, IPC_typeFile);
    if(!h) return NULL;

    h->ipc_size = 0;

    h->ipc_descr.ipc_file = open(name, flags, mode);
    if(h->ipc_descr.ipc_file < 0) {

            DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
            return NULL;
    }

    DEBUG_PRINT("%s(): open file - %s\n", __FUNCTION__, name );

    return h;
}

//-----------------------------------------------------------------------------

int IPC_closeFile(IPC_handle handle)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_invalidHandle;

    int res = close(h->ipc_descr.ipc_file);
    if(res < 0) {
            DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
            return -1;
    }

    DEBUG_PRINT("%s(): close file - %s\n", __FUNCTION__, h->ipc_name );

    delete_ipc_object(h);

    return 0;
}

//-----------------------------------------------------------------------------

int IPC_readFile(IPC_handle handle, void *data, int size)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_invalidHandle;

    int res = read(h->ipc_descr.ipc_file,data,size);
    if(res < 0) {
        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
        return -1;
    }

    return res;
}

//-----------------------------------------------------------------------------

int IPC_writeFile(IPC_handle handle, void *data, int size)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_invalidHandle;

    int res = write(h->ipc_descr.ipc_file,data,size);
    if(res < 0) {
        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
        return -1;
    }

    return res;
}

//-----------------------------------------------------------------------------

IPC_handle IPC_openDevice(IPC_str *devname, const IPC_str *mainname, int devnum);
{
    DEBUG_PRINT("%s(%s)\n", __FUNCTION__, mainname);

    if(!mainname) return NULL;

    snprintf( devname, 128, "%s%s%d", "/dev/", mainname, devnum);
    ipc_handle_t h = allocate_ipc_object(devname, IPC_typeFile);
    if(!h) return NULL;

    h->ipc_size = 0;

    h->ipc_descr.ipc_file = open(devname, S_IROTH | S_IWOTH );
    if(h->ipc_descr.ipc_file < 0) {

            DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
            return NULL;
    }

    DEBUG_PRINT("%s(): open file - %s\n", __FUNCTION__, devname );

    return h;
}

//-----------------------------------------------------------------------------

int IPC_closeDevice(IPC_handle handle)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_invalidHandle;

    int res = close(h->ipc_descr.ipc_file);
    if(res < 0) {
            DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
            return -1;
    }

    DEBUG_PRINT("%s(): close file - %s\n", __FUNCTION__, h->ipc_name );

    delete_ipc_object(h);

    return 0;
}

//-----------------------------------------------------------------------------

int IPC_readDevice(IPC_handle handle, void *data, int size)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_invalidHandle;

    int res = read(h->ipc_descr.ipc_file,data,size);
    if(res < 0) {
        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
        return -1;
    }

    return res;
}

//-----------------------------------------------------------------------------

int IPC_writeDevice(IPC_handle handle, void *data, int size)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_invalidHandle;

    int res = write(h->ipc_descr.ipc_file,data,size);
    if(res < 0) {
        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
        return -1;
    }

    return res;
}

//-----------------------------------------------------------------------------

int IPC_ioctlDevice(IPC_handle handle, unsigned long cmd, void *param)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_invalidHandle;

    int res = ioctl(h->ipc_descr.ipc_file,cmd,param);
    if(res < 0) {
        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
        return -1;
    }

    return res;
}

//-----------------------------------------------------------------------------

#endif //__IPC_LINUX__

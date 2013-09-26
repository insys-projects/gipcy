
#ifdef __IPC_QNX__

#ifndef __QNXIPC_H__
    #include "qnxipc.h"
#endif
#ifndef __GIPCY_H__
    #include "gipcy.h"
#endif
#ifndef __IPCIOCTL_H__
    #include "ipcioctl.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

//-----------------------------------------------------------------------------

IPC_handle IPC_createMutex(const IPC_str *name, bool value)
{
    int fd = ipc_driver_handle();
    if(fd < 0) {
        DEBUG_PRINT("%s(): IPC driver was not opened\n", __FUNCTION__);
        return NULL;
    }

    ipc_create_t ipc_param;

    memset(&ipc_param,0,sizeof(ipc_param));

    ipc_param.handle = NULL;
    ipc_param.value = value ? 0 : 1;
    snprintf(ipc_param.name, sizeof(ipc_param.name), "%s", name);

    int res = devctl(fd,IOCTL_IPC_MUTEX_OPEN,&ipc_param, sizeof(ipc_param), NULL);
    if(res < 0) {
        DEBUG_PRINT("%s(): Error open mutex - %s\n", __FUNCTION__, name);
        return NULL;
    }

    if(ipc_param.handle)
        return ipc_param.handle;

    return NULL;
}

//-----------------------------------------------------------------------------

int IPC_captureMutex(const IPC_handle handle, int timeout)
{
    int fd = ipc_driver_handle();
    if(fd < 0) {
        DEBUG_PRINT("%s(): IPC driver was not opened\n", __FUNCTION__);
        return -1;
    }

    ipc_lock_t ipc_param;

    memset(&ipc_param,0,sizeof(ipc_param));

    ipc_param.handle = handle;
    ipc_param.timeout = timeout;

    int res = devctl(fd, IOCTL_IPC_MUTEX_LOCK, &ipc_param, sizeof(ipc_param), NULL);
    if(res < 0) {
        DEBUG_PRINT("%s(): Error lock mutex\n", __FUNCTION__);
        return -1;
    }

    return IPC_OK;
}

//-----------------------------------------------------------------------------

int IPC_releaseMutex(const IPC_handle handle)
{
    int fd = ipc_driver_handle();
    if(fd < 0) {
        DEBUG_PRINT("%s(): IPC driver was not opened\n", __FUNCTION__);
        return -1;
    }

    ipc_unlock_t ipc_param;

    memset(&ipc_param,0,sizeof(ipc_param));

    ipc_param.handle = handle;

    int res = devctl(fd, IOCTL_IPC_MUTEX_UNLOCK, &ipc_param, sizeof(ipc_param), NULL);
    if(res < 0) {
        DEBUG_PRINT("%s(): Error unlock mutex\n", __FUNCTION__);
        return -1;
    }

    return IPC_OK;
}

//-----------------------------------------------------------------------------

int IPC_deleteMutex(IPC_handle handle)
{
    int fd = ipc_driver_handle();
    if(fd < 0) {
        DEBUG_PRINT("%s(): IPC driver was not opened\n", __FUNCTION__);
        return -1;
    }

    ipc_close_t ipc_param;

    memset(&ipc_param,0,sizeof(ipc_param));

    ipc_param.handle = handle;

    int res = devctl(fd, IOCTL_IPC_MUTEX_CLOSE, &ipc_param, sizeof(ipc_param), NULL);
    if(res < 0) {
        DEBUG_PRINT("%s(): Error close mutex\n", __FUNCTION__);
        return -1;
    }

    return IPC_OK;
}

//-----------------------------------------------------------------------------

#endif //__IPC_QNX__

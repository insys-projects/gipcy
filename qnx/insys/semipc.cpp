
#ifdef __IPC_QNX__

#ifndef __QNXPC_H__
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

IPC_handle IPC_createSemaphore(const IPC_str *name, int value)
{
    int fd = ipc_driver_handle();
    if(fd < 0) {
        DEBUG_PRINT("%s(): IPC driver was not opened\n", __FUNCTION__);
        return NULL;
    }

    ipc_create_t ipc_param;

    memset(&ipc_param,0,sizeof(ipc_param));

    ipc_param.handle = NULL;
    ipc_param.value = 1;
    snprintf(ipc_param.name, sizeof(ipc_param.name), "%s\n", name);

    int res = devctl(fd, IOCTL_IPC_SEM_OPEN, &ipc_param, sizeof(ipc_param), NULL);
    if(res < 0) {
        DEBUG_PRINT("%s(): Error open semaphore - %s\n", __FUNCTION__, name);
        return NULL;
    }

    if(ipc_param.handle)
        return ipc_param.handle;

    return NULL;
}

//-----------------------------------------------------------------------------

int IPC_lockSemaphore(const  IPC_handle handle, int timeout)
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

    int res = devctl(fd, IOCTL_IPC_SEM_LOCK, &ipc_param, sizeof(ipc_param), NULL);
    if(res < 0) {
        DEBUG_PRINT("%s(): Error lock semaphore\n", __FUNCTION__);
        return res;
    }

    return IPC_OK;
}

//-----------------------------------------------------------------------------

int IPC_unlockSemaphore(const  IPC_handle handle)
{
    int fd = ipc_driver_handle();
    if(fd < 0) {
        DEBUG_PRINT("%s(): IPC driver was not opened\n", __FUNCTION__);
        return -1;
    }

    ipc_unlock_t ipc_param;

    memset(&ipc_param,0,sizeof(ipc_param));

    ipc_param.handle = handle;

    int res = devctl(fd, IOCTL_IPC_SEM_UNLOCK, &ipc_param, sizeof(ipc_param), NULL);
    if(res < 0) {
        DEBUG_PRINT("%s(): Error unlock semaphore\n", __FUNCTION__);
        return -1;
    }

    return IPC_OK;
}

//-----------------------------------------------------------------------------

int IPC_deleteSemaphore(IPC_handle handle)
{
    int fd = ipc_driver_handle();
    if(fd < 0) {
        DEBUG_PRINT("%s(): IPC driver was not opened\n", __FUNCTION__);
        return -1;
    }

    ipc_close_t ipc_param;

    memset(&ipc_param,0,sizeof(ipc_param));

    ipc_param.handle = handle;

    int res = devctl(fd, IOCTL_IPC_SEM_CLOSE, &ipc_param, sizeof(ipc_param), NULL);
    if(res < 0) {
        DEBUG_PRINT("%s(): Error close semaphore\n", __FUNCTION__);
        return -1;
    }

    return IPC_OK;
}

//-----------------------------------------------------------------------------

#endif //__IPC_QNX__

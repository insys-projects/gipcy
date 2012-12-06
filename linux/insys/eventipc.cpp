
#ifdef __IPC_LINUX__

#ifndef __LINIPC_H__
    #include "linipc.h"
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
#include <error.h>
#include <time.h>

//-----------------------------------------------------------------------------

IPC_handle IPC_createEvent(const IPC_str *name, bool manual, bool value)
{
    int fd = ipc_driver_handle();
    if(fd < 0) {
        DEBUG_PRINT("%s(): IPC driver was not opened\n", __FUNCTION__);
        return NULL;
    }

    ipc_create_t ipc_param;

    memset(&ipc_param,0,sizeof(ipc_param));

    ipc_param.handle = NULL;
    ipc_param.value = value ? 1 : 0;
    ipc_param.flag = manual ? 1 : 0;
    snprintf(ipc_param.name, sizeof(ipc_param.name), "%s", name);

    int res = ioctl(fd,IOCTL_IPC_EVENT_OPEN,&ipc_param);
    if(res < 0) {
        DEBUG_PRINT("%s(): Error open semaphore - %s\n", __FUNCTION__, name);
        return NULL;
    }

    if(ipc_param.handle)
        return ipc_param.handle;

    return NULL;
}

//-----------------------------------------------------------------------------

int IPC_waitEvent(const  IPC_handle handle, int timeout)
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

    int res = ioctl(fd, IOCTL_IPC_EVENT_LOCK, &ipc_param);
    if(res < 0) {
        DEBUG_PRINT("%s(): Error close semaphore\n", __FUNCTION__);
        return -1;
    }

    return IPC_OK;
}

//-----------------------------------------------------------------------------

int IPC_setEvent(const  IPC_handle handle)
{
    int fd = ipc_driver_handle();
    if(fd < 0) {
        DEBUG_PRINT("%s(): IPC driver was not opened\n", __FUNCTION__);
        return -1;
    }

    ipc_unlock_t ipc_param;

    memset(&ipc_param,0,sizeof(ipc_param));

    ipc_param.handle = handle;

    int res = ioctl(fd, IOCTL_IPC_EVENT_UNLOCK, &ipc_param);
    if(res < 0) {
        DEBUG_PRINT("%s(): Error close semaphore\n", __FUNCTION__);
        return -1;
    }

    return IPC_OK;
}

//-----------------------------------------------------------------------------

int IPC_resetEvent(const  IPC_handle handle)
{
    int fd = ipc_driver_handle();
    if(fd < 0) {
        DEBUG_PRINT("%s(): IPC driver was not opened\n", __FUNCTION__);
        return -1;
    }

    ipc_unlock_t ipc_param;

    memset(&ipc_param,0,sizeof(ipc_param));

    ipc_param.handle = handle;

    int res = ioctl(fd, IOCTL_IPC_EVENT_RESET, &ipc_param);
    if(res < 0) {
        DEBUG_PRINT("%s(): Error close semaphore\n", __FUNCTION__);
        return -1;
    }

    return IPC_OK;
}

//-----------------------------------------------------------------------------

int IPC_deleteEvent(IPC_handle handle)
{
    int fd = ipc_driver_handle();
    if(fd < 0) {
        DEBUG_PRINT("%s(): IPC driver was not opened\n", __FUNCTION__);
        return -1;
    }

    ipc_close_t ipc_param;

    memset(&ipc_param,0,sizeof(ipc_param));

    ipc_param.handle = handle;

    int res = ioctl(fd, IOCTL_IPC_EVENT_CLOSE, &ipc_param);
    if(res < 0) {
        DEBUG_PRINT("%s(): Error close semaphore\n", __FUNCTION__);
        return -1;
    }

    return IPC_OK;
}

//-----------------------------------------------------------------------------

#endif //__IPC_LINUX__
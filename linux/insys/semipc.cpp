
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

IPC_handle IPC_createSemaphore(const IPC_str *name, int value)
{
    int fd = ipc_driver_handle();
    if(fd < 0) {
        DEBUG_PRINT("%s(): IPC driver was not opened\n", __FUNCTION__);
        return NULL;
    }

    sem_create_t sem_param;

    memset(&sem_param,0,sizeof(sem_param));

    sem_param.handle = NULL;
    sem_param.value = 1;
    snprintf(sem_param.name, sizeof(sem_param.name), "%s", name);

    int res = ioctl(fd,IOCTL_IPC_SEM_OPEN,&sem_param);
    if(res < 0) {
        DEBUG_PRINT("%s(): Error open semaphore - %s\n", __FUNCTION__, name);
        return NULL;
    }

    if(sem_param.handle)
        return sem_param.handle;

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

    sem_lock_t sem_param;

    memset(&sem_param,0,sizeof(sem_param));

    sem_param.handle = handle;
    sem_param.timeout = timeout;

    int res = ioctl(fd, IOCTL_IPC_SEM_WAIT, &sem_param);
    if(res < 0) {
        DEBUG_PRINT("%s(): Error close semaphore\n", __FUNCTION__);
        return -1;
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

    sem_unlock_t sem_param;

    memset(&sem_param,0,sizeof(sem_param));

    sem_param.handle = handle;

    int res = ioctl(fd, IOCTL_IPC_SEM_POST, &sem_param);
    if(res < 0) {
        DEBUG_PRINT("%s(): Error close semaphore\n", __FUNCTION__);
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

    sem_close_t sem_param;

    memset(&sem_param,0,sizeof(sem_param));

    sem_param.handle = handle;

    int res = ioctl(fd, IOCTL_IPC_SEM_CLOSE, &sem_param);
    if(res < 0) {
        DEBUG_PRINT("%s(): Error close semaphore\n", __FUNCTION__);
        return -1;
    }

    return IPC_OK;
}

//-----------------------------------------------------------------------------

#endif //__IPC_LINUX__

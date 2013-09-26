
#ifndef __IPCIOTCL_H__
#define __IPCIOTCL_H__

//-----------------------------------------------------------------------------

#define IPC_DRIVER_NAME             "ipc_driver"
#define MAX_IPCDEVICE_SUPPORT       1

//-----------------------------------------------------------------------------

#ifdef __QNX__
#include <devctl.h>
#define IPC_MAKE_IOCTL(cmd) __DION(_DCMD_ALL, cmd)
#endif

#define IOCTL_IPC_SEM_OPEN		IPC_MAKE_IOCTL(10)
#define IOCTL_IPC_SEM_LOCK		IPC_MAKE_IOCTL(11)
#define IOCTL_IPC_SEM_UNLOCK            IPC_MAKE_IOCTL(12)
#define IOCTL_IPC_SEM_CLOSE		IPC_MAKE_IOCTL(13)

#define IOCTL_IPC_MUTEX_OPEN		IPC_MAKE_IOCTL(20)
#define IOCTL_IPC_MUTEX_LOCK		IPC_MAKE_IOCTL(21)
#define IOCTL_IPC_MUTEX_UNLOCK          IPC_MAKE_IOCTL(22)
#define IOCTL_IPC_MUTEX_CLOSE		IPC_MAKE_IOCTL(23)

#define IOCTL_IPC_EVENT_OPEN		IPC_MAKE_IOCTL(30)
#define IOCTL_IPC_EVENT_LOCK		IPC_MAKE_IOCTL(31)
#define IOCTL_IPC_EVENT_UNLOCK          IPC_MAKE_IOCTL(32)
#define IOCTL_IPC_EVENT_RESET           IPC_MAKE_IOCTL(33)
#define IOCTL_IPC_EVENT_CLOSE		IPC_MAKE_IOCTL(34)

#define IOCTL_IPC_SHM_OPEN		IPC_MAKE_IOCTL(40)
#define IOCTL_IPC_SHM_CLOSE             IPC_MAKE_IOCTL(41)

//-----------------------------------------------------------------------------

struct ipc_create_t {

    char    name[128];
    int     value;
    int     flag;
    void    *handle;
};

//-----------------------------------------------------------------------------

struct ipc_lock_t {

    void    *handle;
    int     timeout;
};

//-----------------------------------------------------------------------------

struct ipc_unlock_t {

    void    *handle;
};

//-----------------------------------------------------------------------------

struct ipc_reset_t {

    void    *handle;
};

//-----------------------------------------------------------------------------

struct ipc_close_t {

    void    *handle;
};

//-----------------------------------------------------------------------------

#endif //_IPCIOTCL_H_

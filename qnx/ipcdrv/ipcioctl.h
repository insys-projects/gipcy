
#ifndef __IPCIOTCL_H__
#define __IPCIOTCL_H__

//-----------------------------------------------------------------------------
#include <devctl.h>
//-----------------------------------------------------------------------------

#define IPC_DRIVER_NAME             "ipc_driver"
#define MAX_IPCDEVICE_SUPPORT       1

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

#define IPC_MAKE_IOCTL(cmd, type) __DIOTF(_DCMD_ALL, cmd, type)

#define IOCTL_IPC_SEM_OPEN		IPC_MAKE_IOCTL(10, struct ipc_create_t)
#define IOCTL_IPC_SEM_LOCK		IPC_MAKE_IOCTL(11, struct ipc_lock_t)
#define IOCTL_IPC_SEM_UNLOCK    IPC_MAKE_IOCTL(12, struct ipc_unlock_t)
#define IOCTL_IPC_SEM_CLOSE		IPC_MAKE_IOCTL(13, struct ipc_close_t)

#define IOCTL_IPC_MUTEX_OPEN	IPC_MAKE_IOCTL(20, struct ipc_create_t)
#define IOCTL_IPC_MUTEX_LOCK	IPC_MAKE_IOCTL(21, struct ipc_lock_t)
#define IOCTL_IPC_MUTEX_UNLOCK  IPC_MAKE_IOCTL(22, struct ipc_unlock_t)
#define IOCTL_IPC_MUTEX_CLOSE	IPC_MAKE_IOCTL(23, struct ipc_close_t)

#define IOCTL_IPC_EVENT_OPEN	IPC_MAKE_IOCTL(30, struct ipc_create_t)
#define IOCTL_IPC_EVENT_LOCK	IPC_MAKE_IOCTL(31, struct ipc_lock_t)
#define IOCTL_IPC_EVENT_UNLOCK  IPC_MAKE_IOCTL(32, struct ipc_unlock_t)
#define IOCTL_IPC_EVENT_RESET   IPC_MAKE_IOCTL(33, struct ipc_reset_t)
#define IOCTL_IPC_EVENT_CLOSE	IPC_MAKE_IOCTL(34, struct ipc_close_t)

#define IOCTL_IPC_SHM_OPEN		IPC_MAKE_IOCTL(40, struct ipc_create_t)
#define IOCTL_IPC_SHM_CLOSE     IPC_MAKE_IOCTL(41, struct ipc_close_t)

//-----------------------------------------------------------------------------


#endif //_IPCIOTCL_H_

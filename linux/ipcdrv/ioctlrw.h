
#ifndef _IOCTLRW_H_
#define _IOCTLRW_H_

//-----------------------------------------------------------------------------

int ioctl_ipc_open(struct ipc_driver *drv, unsigned long arg);
int ioctl_ipc_close(struct ipc_driver *drv, unsigned long arg);
int ioctl_ipc_lock(struct ipc_driver *drv, unsigned long arg);
int ioctl_ipc_unlock(struct ipc_driver *drv, unsigned long arg);
int ioctl_ipc_reset(struct ipc_driver *drv, unsigned long arg);

//-----------------------------------------------------------------------------

#endif //_IOCTLRW_H_

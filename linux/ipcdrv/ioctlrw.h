
#ifndef _IOCTLRW_H_
#define _IOCTLRW_H_

//-----------------------------------------------------------------------------

int ioctl_sem_open(struct ipc_driver *drv, unsigned long arg);
int ioctl_sem_close(struct ipc_driver *drv, unsigned long arg);
int ioctl_sem_down(struct ipc_driver *drv, unsigned long arg);
int ioctl_sem_up(struct ipc_driver *drv, unsigned long arg);

//-----------------------------------------------------------------------------

#endif //_IOCTLRW_H_

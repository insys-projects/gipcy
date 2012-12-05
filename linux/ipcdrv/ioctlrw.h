
#ifndef _IOCTLRW_H_
#define _IOCTLRW_H_

//-----------------------------------------------------------------------------

int ioctl_sem_open(struct ipc_driver *drv, unsigned long arg);
int ioctl_sem_close(struct ipc_driver *drv, unsigned long arg);
int ioctl_sem_lock(struct ipc_driver *drv, unsigned long arg);
int ioctl_sem_unlock(struct ipc_driver *drv, unsigned long arg);

int ioctl_mutex_open(struct ipc_driver *drv, unsigned long arg);
int ioctl_mutex_close(struct ipc_driver *drv, unsigned long arg);
int ioctl_mutex_lock(struct ipc_driver *drv, unsigned long arg);
int ioctl_mutex_unlock(struct ipc_driver *drv, unsigned long arg);

int ioctl_event_open(struct ipc_driver *drv, unsigned long arg);
int ioctl_event_close(struct ipc_driver *drv, unsigned long arg);
int ioctl_event_lock(struct ipc_driver *drv, unsigned long arg);
int ioctl_event_unlock(struct ipc_driver *drv, unsigned long arg);
int ioctl_event_reset(struct ipc_driver *drv, unsigned long arg);

int ioctl_shm_open(struct ipc_driver *drv, unsigned long arg);
int ioctl_shm_close(struct ipc_driver *drv, unsigned long arg);

//-----------------------------------------------------------------------------

#endif //_IOCTLRW_H_

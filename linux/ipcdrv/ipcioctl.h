
#ifndef __IPCIOTCL_H__
#define __IPCIOTCL_H__

//-----------------------------------------------------------------------------

#define IPC_DRIVER_NAME             "ipc_driver"
#define MAX_IPCDEVICE_SUPPORT       1

//-----------------------------------------------------------------------------

#ifdef __linux__
#include <linux/types.h>
#ifndef __KERNEL__
#include <sys/ioctl.h>
#endif
#define IPC_DEVICE_TYPE             'i'
#define IPC_MAKE_IOCTL(c) _IO(IPC_DEVICE_TYPE, (c))
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

#ifdef __linux__
#ifdef __KERNEL__
    //! Описывает параметры для команд управления устройством
    struct ioctl_param {
        void *srcBuf;       //!< буфер с данными для устройства (через него передаются данные В драйвер нулевого кольца)
        int srcSize;        //!< размер буфера с данными для устройства
        void *dstBuf;       //!< буфер с данными от устройства  (через него передаются данные ИЗ драйвера нулевого кольца)
        int dstSize;        //!< dstSize - размер буфера с данными от устройства
    };
#endif
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

struct ipc_create_t {

    char    name[128];
    int     value;
    int     flag;
    int     lockerid;
    void    *handle;
};

//-----------------------------------------------------------------------------

struct ipc_lock_t {

    void    *handle;
    int     timeout;
    int     lockerid;
};

//-----------------------------------------------------------------------------

struct ipc_unlock_t {

    void    *handle;
    int     lockerid;
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

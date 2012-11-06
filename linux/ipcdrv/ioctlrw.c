
#include <linux/kernel.h>
#define __NO_VERSION__
#include <linux/module.h>
#include <linux/types.h>
#include <linux/ioport.h>
#include <linux/pci.h>
#include <linux/pagemap.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <asm/io.h>

#include "ipcmodule.h"

//-----------------------------------------------------------------------------
/*
static int ipc_ioctl_param(struct ioctl_param *param, unsigned long arg)
{
    if(!param)
        return -EFAULT;

    memset(param, 0, sizeof(struct ioctl_param));

    if(copy_from_user(param, (void *)arg, sizeof(struct ioctl_param))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        return -EFAULT;
    }

    dbg_msg(dbg_trace, "%s(): param->srcBuf = %p\n", __FUNCTION__, param->srcBuf);
    dbg_msg(dbg_trace, "%s(): param->srcSize = 0x%x\n", __FUNCTION__, param->srcSize);
    dbg_msg(dbg_trace, "%s(): param->dstBuf = %p\n", __FUNCTION__, param->dstBuf);
    dbg_msg(dbg_trace, "%s(): param->dstSize = 0x%x\n", __FUNCTION__, param->dstSize);

    return 0;
}
*/
//-----------------------------------------------------------------------------

int ioctl_sem_open(struct ipc_driver *drv, unsigned long arg)
{
    int error = 0;
    struct sem_create_t sem_param;

    if(copy_from_user(&sem_param, (void *)arg, sizeof(struct sem_create_t))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        error = -EFAULT;
        goto do_exit;
    }

    sem_param.handle = ipc_sem_create( drv, &sem_param );
    if(!sem_param.handle) {
        err_msg(err_trace, "%s(): Error in ipc_sem_create()\n", __FUNCTION__);
        error = -EINVAL;
        goto do_exit;
    }

    if(copy_to_user((void*)arg, (void*)&sem_param, sizeof(struct sem_create_t))) {
        err_msg(err_trace, "%s(): Error in copy_to_user()\n", __FUNCTION__);
        error = -EFAULT;
        goto do_exit;
    }

do_exit:
    return error;
}

//-----------------------------------------------------------------------------

int ioctl_sem_close(struct ipc_driver *drv, unsigned long arg)
{
    int error = 0;
    struct sem_close_t sem_param;

    if(copy_from_user(&sem_param, (void *)arg, sizeof(struct sem_close_t))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        error = -EFAULT;
        goto do_exit;
    }

    error = ipc_sem_close( drv, &sem_param );
    if(error < 0) {
        err_msg(err_trace, "%s(): Error in ipc_sem_close()\n", __FUNCTION__);
        goto do_exit;
    }

do_exit:
    return error;
}

//-----------------------------------------------------------------------------

int ioctl_sem_down(struct ipc_driver *drv, unsigned long arg)
{
    int error = 0;
    struct sem_lock_t sem_param;

    if(copy_from_user(&sem_param, (void *)arg, sizeof(struct sem_lock_t))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        error = -EFAULT;
        goto do_exit;
    }

    error = ipc_sem_down( drv, &sem_param );
    if(error < 0) {
        err_msg(err_trace, "%s(): Error in ipc_sem_down()\n", __FUNCTION__);
        goto do_exit;
    }

do_exit:
    return error;
}

//-----------------------------------------------------------------------------

int ioctl_sem_up(struct ipc_driver *drv, unsigned long arg)
{
    int error = 0;
    struct sem_unlock_t sem_param;

    if(copy_from_user(&sem_param, (void *)arg, sizeof(struct sem_unlock_t))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        error = -EFAULT;
        goto do_exit;
    }

    error = ipc_sem_up( drv, &sem_param );
    if(error < 0) {
        err_msg(err_trace, "%s(): Error in ipc_sem_up()\n", __FUNCTION__);
        goto do_exit;
    }

do_exit:
    return error;
}

//-----------------------------------------------------------------------------

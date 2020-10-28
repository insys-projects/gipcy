
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
    struct ipc_create_t ipc_param;

    if(copy_from_user(&ipc_param, (void *)arg, sizeof(struct ipc_create_t))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        error = -EFAULT;
        goto do_exit;
    }

    ipc_param.handle = ipc_sem_create( drv, &ipc_param );
    if(!ipc_param.handle) {
        err_msg(err_trace, "%s(): Error in ipc_sem_create()\n", __FUNCTION__);
        error = -EINVAL;
        goto do_exit;
    }

    if(copy_to_user((void*)arg, (void*)&ipc_param, sizeof(struct ipc_create_t))) {
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
    struct ipc_close_t ipc_param;

    if(copy_from_user(&ipc_param, (void *)arg, sizeof(struct ipc_close_t))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        error = -EFAULT;
        goto do_exit;
    }

    error = ipc_sem_close( drv, &ipc_param );
    if(error < 0) {
        err_msg(err_trace, "%s(): Error in ipc_sem_close()\n", __FUNCTION__);
        goto do_exit;
    }

do_exit:
    return error;
}

//-----------------------------------------------------------------------------

int ioctl_sem_lock(struct ipc_driver *drv, unsigned long arg)
{
    int error = 0;
    struct ipc_lock_t ipc_param;

    if(copy_from_user(&ipc_param, (void *)arg, sizeof(struct ipc_lock_t))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        error = -EFAULT;
        goto do_exit;
    }

    error = ipc_sem_lock( drv, &ipc_param );
    if(error < 0) {
        err_msg(err_trace, "%s(): Error in ipc_sem_down()\n", __FUNCTION__);
        goto do_exit;
    }

do_exit:
    return error;
}

//-----------------------------------------------------------------------------

int ioctl_sem_unlock(struct ipc_driver *drv, unsigned long arg)
{
    int error = 0;
    struct ipc_unlock_t ipc_param;

    if(copy_from_user(&ipc_param, (void *)arg, sizeof(struct ipc_unlock_t))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        error = -EFAULT;
        goto do_exit;
    }

    error = ipc_sem_unlock( drv, &ipc_param );
    if(error < 0) {
        err_msg(err_trace, "%s(): Error in ipc_sem_up()\n", __FUNCTION__);
        goto do_exit;
    }

do_exit:
    return error;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int ioctl_mutex_open(struct ipc_driver *drv, unsigned long arg)
{
    int error = 0;
    struct ipc_create_t ipc_param;

    if(copy_from_user(&ipc_param, (void *)arg, sizeof(struct ipc_create_t))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        error = -EFAULT;
        goto do_exit;
    }

    ipc_param.handle = ipc_mutex_create( drv, &ipc_param );
    if(!ipc_param.handle) {
        err_msg(err_trace, "%s(): Error in ipc_mutex_create()\n", __FUNCTION__);
        error = -EINVAL;
        goto do_exit;
    }

    if(copy_to_user((void*)arg, (void*)&ipc_param, sizeof(struct ipc_create_t))) {
        err_msg(err_trace, "%s(): Error in copy_to_user()\n", __FUNCTION__);
        error = -EFAULT;
        goto do_exit;
    }

do_exit:
    return error;
}

//-----------------------------------------------------------------------------

int ioctl_mutex_close(struct ipc_driver *drv, unsigned long arg)
{
    int error = 0;
    struct ipc_close_t ipc_param;

    if(copy_from_user(&ipc_param, (void *)arg, sizeof(struct ipc_close_t))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        error = -EFAULT;
        goto do_exit;
    }

    error = ipc_mutex_close( drv, &ipc_param );
    if((error < 0) && (error != -EBUSY)) {
        err_msg(err_trace, "%s(): Error in ipc_mutex_close()\n", __FUNCTION__);
        goto do_exit;
    } else {
        error =0;
    }

do_exit:
    return error;
}

//-----------------------------------------------------------------------------

int ioctl_mutex_lock(struct ipc_driver *drv, unsigned long arg)
{
    int error = 0;
    struct ipc_lock_t ipc_param;

    if(copy_from_user(&ipc_param, (void *)arg, sizeof(struct ipc_lock_t))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        error = -EFAULT;
        goto do_exit;
    }

    error = ipc_mutex_lock( drv, &ipc_param );
    if(error < 0) {
        err_msg(err_trace, "%s(): Error in ipc_mutex_lock()\n", __FUNCTION__);
        goto do_exit;
    }

do_exit:
    return error;
}

//-----------------------------------------------------------------------------

int ioctl_mutex_unlock(struct ipc_driver *drv, unsigned long arg)
{
    int error = 0;
    struct ipc_unlock_t ipc_param;

    if(copy_from_user(&ipc_param, (void *)arg, sizeof(struct ipc_unlock_t))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        error = -EFAULT;
        goto do_exit;
    }

    error = ipc_mutex_unlock( drv, &ipc_param );
    if(error < 0) {
        err_msg(err_trace, "%s(): Error in ipc_mutex_unlock()\n", __FUNCTION__);
        goto do_exit;
    }

do_exit:
    return error;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int ioctl_event_open(struct ipc_driver *drv, unsigned long arg)
{
    int error = 0;
    struct ipc_create_t ipc_param;

    if(copy_from_user(&ipc_param, (void *)arg, sizeof(struct ipc_create_t))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        error = -EFAULT;
        goto do_exit;
    }

    ipc_param.handle = ipc_event_create( drv, &ipc_param );
    if(!ipc_param.handle) {
        err_msg(err_trace, "%s(): Error in ipc_event_create()\n", __FUNCTION__);
        error = -EINVAL;
        goto do_exit;
    }

    if(copy_to_user((void*)arg, (void*)&ipc_param, sizeof(struct ipc_create_t))) {
        err_msg(err_trace, "%s(): Error in copy_to_user()\n", __FUNCTION__);
        error = -EFAULT;
        goto do_exit;
    }

do_exit:
    return error;
}

//-----------------------------------------------------------------------------

int ioctl_event_close(struct ipc_driver *drv, unsigned long arg)
{
    int error = 0;
    struct ipc_close_t ipc_param;

    if(copy_from_user(&ipc_param, (void *)arg, sizeof(struct ipc_close_t))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        error = -EFAULT;
        goto do_exit;
    }

    error = ipc_event_close( drv, &ipc_param );
    if(error < 0) {
        err_msg(err_trace, "%s(): Error in ipc_event_close()\n", __FUNCTION__);
        goto do_exit;
    }

do_exit:
    return error;
}

//-----------------------------------------------------------------------------

int ioctl_event_lock(struct ipc_driver *drv, unsigned long arg)
{
    int error = 0;
    struct ipc_lock_t ipc_param;

    if(copy_from_user(&ipc_param, (void *)arg, sizeof(struct ipc_lock_t))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        error = -EFAULT;
        goto do_exit;
    }

    error = ipc_event_lock( drv, &ipc_param );
    if(error < 0) {
        err_msg(err_trace, "%s(): Error in ipc_event_lock()\n", __FUNCTION__);
        goto do_exit;
    }

do_exit:
    return error;
}

//-----------------------------------------------------------------------------

int ioctl_event_unlock(struct ipc_driver *drv, unsigned long arg)
{
    int error = 0;
    struct ipc_unlock_t ipc_param;

    if(copy_from_user(&ipc_param, (void *)arg, sizeof(struct ipc_unlock_t))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        error = -EFAULT;
        goto do_exit;
    }

    error = ipc_event_unlock( drv, &ipc_param );
    if(error < 0) {
        err_msg(err_trace, "%s(): Error in ipc_event_unlock()\n", __FUNCTION__);
        goto do_exit;
    }

do_exit:
    return error;
}

//-----------------------------------------------------------------------------

int ioctl_event_reset(struct ipc_driver *drv, unsigned long arg)
{
    int error = 0;
    struct ipc_reset_t ipc_param;

    if(copy_from_user(&ipc_param, (void *)arg, sizeof(struct ipc_reset_t))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        error = -EFAULT;
        goto do_exit;
    }

    error = ipc_event_reset( drv, &ipc_param );
    if(error < 0) {
        err_msg(err_trace, "%s(): Error in ioctl_event_reset()\n", __FUNCTION__);
        goto do_exit;
    }

do_exit:
    return error;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int ioctl_shm_open(struct ipc_driver *drv, unsigned long arg)
{
    int error = 0;
    struct ipc_create_t ipc_param;

    if(copy_from_user(&ipc_param, (void *)arg, sizeof(struct ipc_create_t))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        error = -EFAULT;
        goto do_exit;
    }

    ipc_param.handle = ipc_shm_open( drv, &ipc_param );
    if(!ipc_param.handle) {
        err_msg(err_trace, "%s(): Error in ipc_shm_open()\n", __FUNCTION__);
        error = -EINVAL;
        goto do_exit;
    }

    if(copy_to_user((void*)arg, (void*)&ipc_param, sizeof(struct ipc_create_t))) {
        err_msg(err_trace, "%s(): Error in copy_to_user()\n", __FUNCTION__);
        error = -EFAULT;
        goto do_exit;
    }

do_exit:
    return error;
}

//-----------------------------------------------------------------------------

int ioctl_shm_close(struct ipc_driver *drv, unsigned long arg)
{
    int error = 0;
    struct ipc_close_t ipc_param;

    if(copy_from_user(&ipc_param, (void *)arg, sizeof(struct ipc_close_t))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        error = -EFAULT;
        goto do_exit;
    }

    error = ipc_shm_close( drv, &ipc_param );
    if(error < 0) {
        err_msg(err_trace, "%s(): Error in ioctl_shm_close()\n", __FUNCTION__);
        goto do_exit;
    }

do_exit:
    return error;
}

//-----------------------------------------------------------------------------

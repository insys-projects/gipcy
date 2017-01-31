
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/ioport.h>
#include <linux/list.h>
#include <linux/pci.h>
#include <linux/proc_fs.h>
#include <linux/interrupt.h>
#include <asm/io.h>

#include <asm/uaccess.h>
#include <linux/types.h>
#include <linux/ioport.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/interrupt.h>

#include "ipcmodule.h"
#include "ipcioctl.h"
#include "ioctlrw.h"
#include "ipcproc.h"

//-----------------------------------------------------------------------------

MODULE_AUTHOR("Vladimir Karakozov. karakozov@gmail.com");
MODULE_LICENSE("GPL");

//-----------------------------------------------------------------------------

static dev_t devno = MKDEV(0, 0);
static struct class *ipc_class = NULL;
static LIST_HEAD(ipc_list);
static struct mutex ipc_mutex;
int dbg_trace = 0;
int err_trace = 1;

#ifndef GIPCY_2_4_X
module_param( dbg_trace, int, S_IRUGO );
module_param( err_trace, int, S_IRUGO );
#endif

//-----------------------------------------------------------------------------

static struct ipc_driver *file_to_device( struct file *file )
{
    return (struct ipc_driver*)file->private_data;
}

//-----------------------------------------------------------------------------
#ifdef GIPCY_2_4_X
static struct ipc_driver *inode_to_device( struct list_head *head, struct inode *inode )
{
    struct list_head *p;
    struct ipc_driver *entry;
    unsigned int minor = MINOR(inode->i_rdev);

    list_for_each(p, head) {
        entry = list_entry(p, struct ipc_driver, m_list);
        if(entry->m_index == minor) {
            return entry;
        }
    }

    return NULL;
}
#endif
//--------------------------------------------------------------------

static int ipc_device_fasync(int fd, struct file *file, int mode)
{
    struct ipc_driver *pDriver = file->private_data;

    dbg_msg(dbg_trace, "%s()\n", __FUNCTION__);

    if(!pDriver)
        return -ENODEV;

    return 0;
}

//-----------------------------------------------------------------------------

static unsigned int ipc_device_poll(struct file *filp, poll_table *wait)
{
    unsigned int mask = 0;
    struct ipc_driver *pDriver = file_to_device(filp);

    dbg_msg(dbg_trace, "%s()\n", __FUNCTION__);

    if(!pDriver)
        return -ENODEV;

    return mask;
}

//-----------------------------------------------------------------------------

static int ipc_device_open( struct inode *inode, struct file *file )
{
#ifdef GIPCY_2_4_X
    struct ipc_driver *pDriver = inode_to_device(&ipc_list, inode);
    if(!pDriver) {
        err_msg(err_trace, "%s(): Open driver failed\n", __FUNCTION__);
        return -ENODEV;
    }
#else
    struct ipc_driver *pDriver = container_of(inode->i_cdev, struct ipc_driver, m_cdev);
    if(!pDriver) {
        err_msg(err_trace, "%s(): Open driver failed\n", __FUNCTION__);
        return -ENODEV;
    }
#endif

    mutex_lock(&pDriver->m_ipc_mutex);

    atomic_inc(&pDriver->m_usage);

    file->private_data = (void*)pDriver;

    dbg_msg(dbg_trace, "%s(): Open driver %s. m_usage = %d. file = %p\n", __FUNCTION__, pDriver->m_name, atomic_read(&pDriver->m_usage), file);

    mutex_unlock(&pDriver->m_ipc_mutex);

    return 0;
}

//-----------------------------------------------------------------------------

static int ipc_device_close( struct inode *inode, struct file *file )
{
#ifdef GIPCY_2_4_X
    struct ipc_driver *pDriver = inode_to_device(&ipc_list, inode);
    if(!pDriver) {
        err_msg(err_trace, "%s(): Close driver failed\n", __FUNCTION__);
        return -ENODEV;
    }
#else
    struct ipc_driver *pDriver = container_of(inode->i_cdev, struct ipc_driver, m_cdev);
    if(!pDriver) {
        err_msg(err_trace, "%s(): Close driver failed\n", __FUNCTION__);
        return -ENODEV;
    }
#endif

    mutex_lock(&pDriver->m_ipc_mutex);

    atomic_dec(&pDriver->m_usage);

    file->private_data = NULL;

    dbg_msg(dbg_trace, "%s(): Close driver %s. m_usage = %d. file = %p\n", __FUNCTION__, pDriver->m_name, atomic_read(&pDriver->m_usage), file);

    if(atomic_read(&pDriver->m_usage) == 0) {
        ipc_sem_close_all(pDriver);
        ipc_mutex_close_all(pDriver);
        ipc_event_close_all(pDriver);
        ipc_shm_close_all(pDriver);
    }

    mutex_unlock(&pDriver->m_ipc_mutex);

    return 0;
}

//-----------------------------------------------------------------------------

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,34)
static long ipc_device_ioctl( struct file *file, unsigned int cmd, unsigned long arg )
#else
static int ipc_device_ioctl( struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg )
#endif
{
    int error = 0;
    struct ipc_driver *pDriver = file_to_device(file);
    if(!pDriver) {
        err_msg(err_trace, "%s(): ioctl driver failed\n", __FUNCTION__);
        return -ENODEV;
    }

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    switch(cmd) {

    case IOCTL_IPC_SEM_OPEN:
        error = ioctl_sem_open(pDriver, arg);
        break;
    case IOCTL_IPC_SEM_LOCK:
        error = ioctl_sem_lock(pDriver, arg);
        break;
    case IOCTL_IPC_SEM_UNLOCK:
        error = ioctl_sem_unlock(pDriver, arg);
        break;
    case IOCTL_IPC_SEM_CLOSE:
        error = ioctl_sem_close(pDriver, arg);
        break;
    case IOCTL_IPC_MUTEX_OPEN:
        error = ioctl_mutex_open(pDriver, arg);
        break;
    case IOCTL_IPC_MUTEX_LOCK:
        error = ioctl_mutex_lock(pDriver, arg);
        break;
    case IOCTL_IPC_MUTEX_UNLOCK:
        error = ioctl_mutex_unlock(pDriver, arg);
        break;
    case IOCTL_IPC_MUTEX_CLOSE:
        error = ioctl_mutex_close(pDriver, arg);
        break;
    case IOCTL_IPC_EVENT_OPEN:
        error = ioctl_event_open(pDriver, arg);
        break;
    case IOCTL_IPC_EVENT_LOCK:
        error = ioctl_event_lock(pDriver, arg);
        break;
    case IOCTL_IPC_EVENT_UNLOCK:
        error = ioctl_event_unlock(pDriver, arg);
        break;
    case IOCTL_IPC_EVENT_CLOSE:
        error = ioctl_event_close(pDriver, arg);
        break;
    case IOCTL_IPC_EVENT_RESET:
        error = ioctl_event_reset(pDriver, arg);
        break;
    case IOCTL_IPC_SHM_OPEN:
        error = ioctl_shm_open(pDriver, arg);
        break;
    case IOCTL_IPC_SHM_CLOSE:
        error = ioctl_shm_close(pDriver, arg);
        break;
    default:
        err_msg(err_trace, "%s(): Unknown ioctl command\n", __FUNCTION__);
        error = -EINVAL;
        break;
    }

    return error;
}

//-----------------------------------------------------------------------------

struct file_operations ipc_fops = {

#ifndef GIPCY_2_4_X
    .owner = THIS_MODULE,
#endif
    .read = NULL,
    .write = NULL,

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,34)
    .unlocked_ioctl = ipc_device_ioctl,
    .compat_ioctl = ipc_device_ioctl,
#else
    .ioctl = ipc_device_ioctl,
#endif

    .mmap = NULL,
    .open = ipc_device_open,
    .release = ipc_device_close,
    .fasync = ipc_device_fasync,
    .poll = ipc_device_poll,
};

//-----------------------------------------------------------------------------

static int  __init ipc_probe(void)
{
    int error = 0;
    struct ipc_driver *drv = NULL;

    dbg_msg(dbg_trace, "%s()\n", __FUNCTION__);

    drv = kzalloc(sizeof(struct ipc_driver), GFP_KERNEL);
    if(!drv) {
        error = -ENOMEM;
        goto do_out;
    }

    INIT_LIST_HEAD(&drv->m_list);

    mutex_init(&drv->m_ipc_mutex);
    sema_init(&drv->m_ipc_sem, 1);
    spin_lock_init(&drv->m_ipc_lock);
    atomic_set(&drv->m_usage, 0);
    drv->m_class = ipc_class;
    drv->m_index = 0;

    //down(&drv->m_ipc_sem);
    //err_msg(err_trace, "%s(): lock sem2222222 %d\n", __FUNCTION__, 0);
    //error = down_trylock(&drv->m_ipc_sem);
    //if(error < 0) {
    //    err_msg(err_trace, "%s(): sem3333333 %d\n", __FUNCTION__, 0);
    //}

    INIT_LIST_HEAD(&drv->m_file_list);
    INIT_LIST_HEAD(&drv->m_sem_list);
    INIT_LIST_HEAD(&drv->m_mutex_list);
    INIT_LIST_HEAD(&drv->m_event_list);
    INIT_LIST_HEAD(&drv->m_shm_list);

    mutex_init(&drv->m_file_lock);
    mutex_init(&drv->m_sem_lock);
    mutex_init(&drv->m_mutex_lock);
    mutex_init(&drv->m_event_lock);
    mutex_init(&drv->m_shm_lock);

    //up(&drv->m_ipc_sem);

    cdev_init(&drv->m_cdev, &ipc_fops);
    drv->m_cdev.owner = THIS_MODULE;
    drv->m_cdev.ops = &ipc_fops;
    drv->m_devno = MKDEV(MAJOR(devno), 0);

    snprintf(drv->m_name, sizeof(drv->m_name), "%s", IPC_DRIVER_NAME);

    error = cdev_add(&drv->m_cdev, drv->m_devno, 1);
    if(error) {
        err_msg(err_trace, "%s(): Error add char device %d\n", __FUNCTION__, 0);
        error = -EINVAL;
        goto do_free_memory;
    }

    dbg_msg(dbg_trace, "%s(): Add cdev %d\n", __FUNCTION__, 0);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,34)
    drv->m_device = device_create(ipc_class, NULL, drv->m_devno, "%s", drv->m_name);
#else
    drv->m_device = device_create(ipc_class, NULL, drv->m_devno, NULL, "%s", drv->m_name);
#endif
    if(!drv->m_device) {
        err_msg(err_trace, "%s(): Error create device for board: %s\n", __FUNCTION__, drv->m_name);
        error = -EINVAL;
        goto do_delete_cdev;
    }

    dbg_msg(dbg_trace, "%s(): Create device file for board: %s\n", __FUNCTION__, drv->m_name);

#ifdef GIPCY_2_4_X
    ipc_register_proc(drv->m_name, ipc_proc_info, drv);
#else
    ipc_register_proc(drv->m_name, NULL, drv);
#endif

    dbg_msg(dbg_trace, "%s(): Driver %s - setup complete\n", __FUNCTION__, drv->m_name);

    list_add_tail(&drv->m_list, &ipc_list);

    return error;

do_delete_cdev:
    cdev_del(&drv->m_cdev);

do_free_memory:
    kfree(drv);

do_out:
    return error;
}

//-----------------------------------------------------------------------------

static void __exit ipc_remove(void)
{
    struct list_head *pos, *n;
    struct ipc_driver *entry = NULL;

    dbg_msg(dbg_trace, "%s()\n", __FUNCTION__);

    list_for_each_safe(pos, n, &ipc_list) {

        entry = list_entry(pos, struct ipc_driver, m_list);

        ipc_sem_close_all(entry);
        ipc_mutex_close_all(entry);
        ipc_event_close_all(entry);
        ipc_shm_close_all(entry);

        ipc_remove_proc(entry->m_name);
        dbg_msg(dbg_trace, "%s(): ipc_remove_proc() - complete\n", __FUNCTION__);
        device_destroy(ipc_class, entry->m_devno);
        dbg_msg(dbg_trace, "%s(): device_destroy() - complete\n", __FUNCTION__);
        cdev_del(&entry->m_cdev);
        dbg_msg(dbg_trace, "%s(): cdev_del() - complete\n", __FUNCTION__);
        list_del(pos);
        dbg_msg(dbg_trace, "%s(): list_del() - complete\n", __FUNCTION__);
        kfree(entry);
        dbg_msg(dbg_trace, "%s(): kfree() - complete\n", __FUNCTION__);
    }
}

//-----------------------------------------------------------------------------

static int __init ipc_module_init(void)
{
    int error = 0;

    dbg_msg(dbg_trace, "%s()\n", __FUNCTION__);

    mutex_init(&ipc_mutex);
    mutex_lock(&ipc_mutex);

    error = alloc_chrdev_region(&devno, 0, MAX_IPCDEVICE_SUPPORT, IPC_DRIVER_NAME);
    if(error < 0) {
        err_msg(err_trace, "%s(): Erorr allocate char device regions\n", __FUNCTION__);
        goto do_out;
    }

    dbg_msg(dbg_trace, "%s(): Allocate %d device numbers. Major number = %d\n", __FUNCTION__, MAX_IPCDEVICE_SUPPORT, MAJOR(devno));

    ipc_class = class_create(THIS_MODULE, IPC_DRIVER_NAME);
    if(!ipc_class) {
        err_msg(err_trace, "%s(): Erorr create IPC class: %s\n", __FUNCTION__, IPC_DRIVER_NAME);
        error = -EINVAL;
        goto do_free_chrdev;
    }

#ifdef GIPCY_2_4_X
    ipc_class->m_fops = &ipc_fops;
#endif

    error = ipc_probe();
    if(error < 0) {
        err_msg(err_trace, "%s(): Erorr probe IPC driver\n", __FUNCTION__);
        error = -EINVAL;
        goto do_delete_class;
    }

    mutex_unlock(&ipc_mutex);

    return 0;

do_delete_class:
    class_destroy(ipc_class);

do_free_chrdev:
    unregister_chrdev_region(devno, MAX_IPCDEVICE_SUPPORT);

do_out:
    mutex_unlock(&ipc_mutex);

    return error;
}

//-----------------------------------------------------------------------------

static void __exit ipc_module_cleanup(void)
{
    dbg_msg(dbg_trace, "%s()\n", __FUNCTION__);

    mutex_lock(&ipc_mutex);

    ipc_remove();

    if(ipc_class)
        class_destroy(ipc_class);

    unregister_chrdev_region(devno, MAX_IPCDEVICE_SUPPORT);

    mutex_unlock(&ipc_mutex);
}

//-----------------------------------------------------------------------------

module_init(ipc_module_init);
module_exit(ipc_module_cleanup);

//-----------------------------------------------------------------------------

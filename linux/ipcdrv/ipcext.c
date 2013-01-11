
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
#include <asm/system.h>
#include <asm/io.h>
#include <asm/io.h>

#include <asm/uaccess.h>
#include <linux/types.h>
#include <linux/ioport.h>
#include <linux/poll.h>
#include <linux/pci.h>
#include <linux/interrupt.h>

#include "ipcmodule.h"

#ifdef DZYTOOLS_2_4_X

//-----------------------------------------------------------------------------

void mutex_init(struct mutex *m)
{
    sema_init(&m->m_sem, 1);
}

//-----------------------------------------------------------------------------

void mutex_lock(struct mutex *m)
{
    down(&m->m_sem);
}

//-----------------------------------------------------------------------------

void mutex_unlock(struct mutex *m)
{
    up(&m->m_sem);
}

//-----------------------------------------------------------------------------

void *kzalloc(int flags, size_t size)
{
    void *ptr = kmalloc(flags, size);
    if(ptr) {
        memset(ptr,0,size);
    }
    return ptr;
}

//-----------------------------------------------------------------------------

int down_timeout(struct semaphore *sem, int timeout)
{
    return down_interruptible(sem);
}

//-----------------------------------------------------------------------------

int cdev_init(struct cdev *cd, struct file_operations *fops)
{
    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    memset(cd, 0, sizeof(struct cdev));
    cd->ops = fops;

    return 0;
}

//-----------------------------------------------------------------------------

int cdev_add(struct cdev *cd, int major, int count)
{
    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    cd->m_major = major;
    cd->m_count = count;

    return 0;
}

//-----------------------------------------------------------------------------

int cdev_del(struct cdev *cd)
{
    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );
    return 0;
}

//-----------------------------------------------------------------------------

struct device* device_create(struct class *class, struct device *parent, dev_t devt, void *drvdata, const char *fmt, ...)
{
    struct device *dev = NULL;

    if(!class) {
        err_msg( err_trace, "%s(): Invalid class pointer.\n", __FUNCTION__ );
        return NULL;
    }

    int major = register_chrdev(0, class->m_drvname, class->m_fops);
    if(major < 0) {
        err_msg( err_trace, "%s(): Error in register_chrdev().\n", __FUNCTION__ );
        return NULL;
    }

    dev = kzalloc(GFP_KERNEL, sizeof(struct device));

    if(dev) {

        dev->m_class = class;
        dev->m_parent = parent;
        dev->m_devno = devt;
        dev->m_data = drvdata;

        class->m_dev = dev;
        class->m_major = major;
    }

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    return dev;
}

//-----------------------------------------------------------------------------

int device_destroy(struct class *class, dev_t devno)
{
    int error = -EINVAL;

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    unregister_chrdev( class->m_major, class->m_drvname );
    kfree(class->m_dev);
    error = 0;

    return error;
}

//-----------------------------------------------------------------------------

int alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count, const char *name)
{
    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );
    return 0;
}

//-----------------------------------------------------------------------------

int unregister_chrdev_region(dev_t from, unsigned count)
{
    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );
    return 0;
}

//-----------------------------------------------------------------------------

struct class* class_create(void* owner, const char *name)
{
    struct class *class = (struct class*)kzalloc(GFP_KERNEL, sizeof(struct class));
    if(class) {
        snprintf(class->m_drvname, sizeof(class->m_drvname), "%s", name);
        class->m_owner = owner;
    }

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    return class;
}

//-----------------------------------------------------------------------------

int class_destroy(struct class *class)
{
    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );
    kfree(class);
    return 0;
}

//-----------------------------------------------------------------------------

#endif //DZYTOOLS_2_4_X

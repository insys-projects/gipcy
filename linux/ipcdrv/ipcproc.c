
#include <linux/kernel.h>
#define __NO_VERSION__
#include <linux/module.h>
#include <linux/types.h>
#include <linux/version.h>
#include <linux/ioport.h>
#include <linux/pci.h>
#include <linux/pagemap.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/io.h>

#include "ipcmodule.h"
#include "ipcproc.h"

//--------------------------------------------------------------------

struct log_buf_t {
#ifdef GIPCY_2_4_X
    char *param;
#else
    struct seq_file *param;
#endif
};

//--------------------------------------------------------------------

#ifdef GIPCY_2_4_X
#define print_info(p, S...) do { p+=sprintf(p,S); } while(0)
#else
#define print_info(S...) seq_printf(S)
#endif

//--------------------------------------------------------------------

#ifndef GIPCY_2_4_X

//--------------------------------------------------------------------

static int show_sem_info( struct ipc_driver *drv, struct seq_file *m )
{
    struct list_head *pos, *n;
    struct ipcsem_t *entry = NULL;
    int sem_counter = 0;

    if(!drv || !m) {
        dbg_msg( dbg_trace, "%s(): EINVAL\n", __FUNCTION__ );
        return -1;
    }

    seq_printf(m, "%s\n", "Semaphores");

    mutex_lock(&drv->m_sem_lock);

    list_for_each_safe(pos, n, &drv->m_sem_list) {

        entry = list_entry(pos, struct ipcsem_t, sem_list);

        if(entry) {

            seq_printf(m, "%d: %s (lock %d, unlock %d, usage %d)\n", sem_counter,
                       entry->sem_name,
                       atomic_read(&entry->sem_lock_count),
                       atomic_read(&entry->sem_unlock_count),
                       atomic_read(&entry->sem_owner_count));
            sem_counter++;
        }
    }

    mutex_unlock(&drv->m_sem_lock);

    seq_printf(m, "Total semaphores: %d\n", sem_counter );

    return sem_counter;
}

//--------------------------------------------------------------------

static int show_mutex_info( struct ipc_driver *drv, struct seq_file *m )
{
    struct list_head *pos, *n;
    struct ipcmutex_t *entry = NULL;
    int mutex_counter = 0;

    if(!drv || !m) {
        dbg_msg( dbg_trace, "%s(): EINVAL\n", __FUNCTION__ );
        return -1;
    }

    seq_printf(m, "Mutexes\n" );

    mutex_lock(&drv->m_mutex_lock);

    list_for_each_safe(pos, n, &drv->m_mutex_list) {

        entry = list_entry(pos, struct ipcmutex_t, mutex_list);

        if(entry) {

            seq_printf(m, "%d: %s (lock %d, unlock %d, usage %d)\n", mutex_counter,
                          entry->mutex_name,
                          atomic_read(&entry->mutex_lock_count),
                          atomic_read(&entry->mutex_unlock_count),
                          atomic_read(&entry->mutex_owner_count));
            mutex_counter++;
        }
    }

    mutex_unlock(&drv->m_mutex_lock);

    seq_printf(m, "Total mutexes: %d\n", mutex_counter );

    return mutex_counter;
}

//--------------------------------------------------------------------

static int show_event_info( struct ipc_driver *drv, struct seq_file *m )
{
    struct list_head *pos, *n;
    struct ipcevent_t *entry = NULL;
    int event_counter = 0;

    if(!drv || !m) {
        dbg_msg( dbg_trace, "%s(): EINVAL\n", __FUNCTION__ );
        return -1;
    }

    seq_printf(m, "Events\n" );

    mutex_lock(&drv->m_event_lock);

    list_for_each_safe(pos, n, &drv->m_event_list) {

        entry = list_entry(pos, struct ipcevent_t, event_list);

        if(entry) {

            seq_printf(m, "%d: %s (lock %d, unlock %d, usage %d)\n", event_counter,
                          entry->event_name,
                          atomic_read(&entry->event_lock_count),
                          atomic_read(&entry->event_unlock_count),
                          atomic_read(&entry->event_owner_count));
            event_counter++;
        }
    }

    mutex_unlock(&drv->m_event_lock);

    seq_printf(m, "Total events: %d\n", event_counter );

    return event_counter;
}

//--------------------------------------------------------------------

static int show_shm_info( struct ipc_driver *drv, struct seq_file *m )
{
    struct list_head *pos, *n;
    struct ipcshm_t *entry = NULL;
    int sem_counter = 0;

    if(!drv || !m) {
        dbg_msg( dbg_trace, "%s(): EINVAL\n", __FUNCTION__ );
        return -1;
    }

    seq_printf(m, "Shared memory\n" );

    mutex_lock(&drv->m_shm_lock);

    list_for_each_safe(pos, n, &drv->m_shm_list) {

        entry = list_entry(pos, struct ipcshm_t, shm_list);

        if(entry) {

            seq_printf(m, "%d: %s (usage %d)\n", sem_counter,
                          entry->shm_name, atomic_read(&entry->shm_owner_count));
            sem_counter++;
        }
    }

    mutex_unlock(&drv->m_shm_lock);

    seq_printf(m, "Total shared memories: %d\n", sem_counter );

    return sem_counter;
}

//--------------------------------------------------------------------

static int ipcdrv_proc_show(struct seq_file *m, void *v)
{
    struct ipc_driver *pDriver = m->private;

    show_shm_info( pDriver, m );
    show_sem_info( pDriver, m );
    show_mutex_info( pDriver, m );
    show_event_info( pDriver, m );

    return 0;
}

//--------------------------------------------------------------------

static int ipcdrv_proc_open(struct inode *inode, struct file *file)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
    struct ipc_driver *pDriver = (struct ipc_driver *)PDE_DATA(inode);
#else
    struct ipc_driver *pDriver = PDE(inode)->data;
#endif

    return single_open(file, ipcdrv_proc_show, pDriver);
}

//--------------------------------------------------------------------

static int ipcdrv_proc_release(struct inode *inode, struct file *file)
{
    return single_release(inode, file);
}

//--------------------------------------------------------------------

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops ipcdrv_proc_fops = {
    .proc_open           = ipcdrv_proc_open,
    .proc_read           = seq_read,
    .proc_lseek          = seq_lseek,
    .proc_release        = ipcdrv_proc_release,
};
#else
static const struct file_operations ipcdrv_proc_fops = {
    .owner          = THIS_MODULE,
    .open           = ipcdrv_proc_open,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = ipcdrv_proc_release,
};
#endif

//--------------------------------------------------------------------

void ipc_register_proc( char *name, void *fptr, void *data )
{
    struct ipc_driver *pDriver = (struct ipc_driver*)data;

    if(!data) {
        dbg_msg( dbg_trace, "%s(): Invalid driver pointer\n", __FUNCTION__ );
        return;
    }

    pDriver->m_proc = proc_create_data(name, S_IRUGO, NULL, &ipcdrv_proc_fops, pDriver);
    if(!pDriver->m_proc) {
        dbg_msg(1, "%s(): Error register /proc entry\n", __FUNCTION__);
    }
}

//--------------------------------------------------------------------

void ipc_remove_proc( char *name )
{
    remove_proc_entry(name, NULL);
}

//--------------------------------------------------------------------

#else

//--------------------------------------------------------------------

static int show_sem_info( struct ipc_driver *drv, char **p )
{
    struct list_head *pos, *n;
    struct ipcsem_t *entry = NULL;
    int sem_counter = 0;

    if(!drv || !*p) {
        dbg_msg( dbg_trace, "%s(): EINVAL\n", __FUNCTION__ );
        return -1;
    }

    *p += sprintf(*p,"Semaphores\n" );

    mutex_lock(&drv->m_sem_lock);

    list_for_each_safe(pos, n, &drv->m_sem_list) {

        entry = list_entry(pos, struct ipcsem_t, sem_list);

        if(entry) {

            *p += sprintf(*p,"%d: %s (lock %d, unlock %d, usage %d)\n", sem_counter,
                          entry->sem_name,
                          atomic_read(&entry->sem_lock_count),
                          atomic_read(&entry->sem_unlock_count),
                          atomic_read(&entry->sem_owner_count));
            sem_counter++;
        }
    }

    mutex_unlock(&drv->m_sem_lock);

    *p += sprintf(*p,"Total semaphores: %d\n", sem_counter );

    return sem_counter;
}

//--------------------------------------------------------------------

static int show_mutex_info( struct ipc_driver *drv, char **p )
{
    struct list_head *pos, *n;
    struct ipcmutex_t *entry = NULL;
    int mutex_counter = 0;

    if(!drv || !*p) {
        dbg_msg( dbg_trace, "%s(): EINVAL\n", __FUNCTION__ );
        return -1;
    }

    *p += sprintf(*p,"Mutexes\n" );

    mutex_lock(&drv->m_mutex_lock);

    list_for_each_safe(pos, n, &drv->m_mutex_list) {

        entry = list_entry(pos, struct ipcmutex_t, mutex_list);

        if(entry) {

            *p += sprintf(*p,"%d: %s (lock %d, unlock %d, usage %d)\n", mutex_counter,
                          entry->mutex_name,
                          atomic_read(&entry->mutex_lock_count),
                          atomic_read(&entry->mutex_unlock_count),
                          atomic_read(&entry->mutex_owner_count));
            mutex_counter++;
        }
    }

    mutex_unlock(&drv->m_mutex_lock);

    *p += sprintf(*p,"Total mutexes: %d\n", mutex_counter );

    return mutex_counter;
}

//--------------------------------------------------------------------

static int show_event_info( struct ipc_driver *drv, char **p )
{
    struct list_head *pos, *n;
    struct ipcevent_t *entry = NULL;
    int event_counter = 0;

    if(!drv || !*p) {
        dbg_msg( dbg_trace, "%s(): EINVAL\n", __FUNCTION__ );
        return -1;
    }

    *p += sprintf(*p,"Events\n" );

    mutex_lock(&drv->m_event_lock);

    list_for_each_safe(pos, n, &drv->m_event_list) {

        entry = list_entry(pos, struct ipcevent_t, event_list);

        if(entry) {

            *p += sprintf(*p,"%d: %s (lock %d, unlock %d, usage %d)\n", event_counter,
                          entry->event_name,
                          atomic_read(&entry->event_lock_count),
                          atomic_read(&entry->event_unlock_count),
                          atomic_read(&entry->event_owner_count));
            event_counter++;
        }
    }

    mutex_unlock(&drv->m_event_lock);

    *p += sprintf(*p,"Total events: %d\n", event_counter );

    return event_counter;
}

//--------------------------------------------------------------------

static int show_shm_info( struct ipc_driver *drv, char **p )
{
    struct list_head *pos, *n;
    struct ipcshm_t *entry = NULL;
    int sem_counter = 0;

    if(!drv || !*p) {
        dbg_msg( dbg_trace, "%s(): EINVAL\n", __FUNCTION__ );
        return -1;
    }

    *p += sprintf(*p,"Shared memory\n" );

    mutex_lock(&drv->m_shm_lock);

    list_for_each_safe(pos, n, &drv->m_shm_list) {

        entry = list_entry(pos, struct ipcshm_t, shm_list);

        if(entry) {

            *p += sprintf(*p,"%d: %s (usage %d)\n", sem_counter,
                          entry->shm_name, atomic_read(&entry->shm_owner_count));
            sem_counter++;
        }
    }

    mutex_unlock(&drv->m_shm_lock);

    *p += sprintf(*p,"Total shared memories: %d\n", sem_counter );

    return sem_counter;
}

//--------------------------------------------------------------------

int ipc_proc_info(  char *buf,
                    char **start,
                    off_t off,
                    int count,
                    int *eof,
                    void *data )
{
    char *p = buf;
    struct ipc_driver *drv = (struct ipc_driver*)data;

    if(!drv) {
        p += sprintf(p,"Invalid driver pointer\n" );
        *eof = 1;
        return p - buf;
    }

    p += sprintf(p,"IPC DRIVER INFO\n" );

    show_shm_info( drv, &p );
    show_sem_info( drv, &p );
    show_mutex_info( drv, &p );
    show_event_info( drv, &p );

    *eof = 1;

    return p - buf;
}

//--------------------------------------------------------------------

void ipc_register_proc( char *name, void *fptr, void *data )
{
    create_proc_read_entry ( name, 0, NULL, fptr, data );
}

//--------------------------------------------------------------------

void ipc_remove_proc( char *name )
{
    remove_proc_entry(name, NULL);
}

//--------------------------------------------------------------------

#endif //GIPCY_2_4_X

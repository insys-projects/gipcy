
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
#include "ipcproc.h"

//--------------------------------------------------------------------

void ipc_register_proc( char *name, void *fptr, void *data )
{
    create_proc_read_entry( name, 0, NULL, fptr, data );
}

//--------------------------------------------------------------------

void ipc_remove_proc( char *name )
{
    remove_proc_entry( name, NULL );
}

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

    *p += sprintf(*p,"Total mutexes: %d\n", event_counter );

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

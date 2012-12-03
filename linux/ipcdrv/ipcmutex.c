
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
#include "ipcioctl.h"

//-----------------------------------------------------------------------------

void* ipc_mutex_create( struct ipc_driver *drv, struct ipc_create_t *param )
{
    bool exist = false;
    struct list_head *pos, *n;
    struct ipcmutex_t *entry = NULL;
    struct ipcmutex_t *mutex = NULL;

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    if(!drv || !param) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    dbg_msg( dbg_trace, "%s(): name = %s\n", __FUNCTION__, param->name );
    dbg_msg( dbg_trace, "%s(): value = %d\n", __FUNCTION__, param->value );

    mutex_lock(&drv->m_mutex_lock);

    list_for_each_safe(pos, n, &drv->m_mutex_list) {

        entry = list_entry(pos, struct ipcmutex_t, mutex_list);

        if(strcmp(entry->mutex_name, param->name) == 0) {
            mutex = entry;
            exist = true;
            break;
        }
    }

    if(!exist) {

        dbg_msg( dbg_trace, "%s(): mutex name = %s was not found. Create new semaphore\n", __FUNCTION__, param->name );

        mutex = (struct ipcmutex_t*)kzalloc(sizeof(struct ipcmutex_t), GFP_KERNEL);
        if(!mutex) {
            err_msg( err_trace, "%s(): Error allocate memory to semaphore\n", __FUNCTION__ );
            goto do_out;
        }

        INIT_LIST_HEAD(&mutex->mutex_list);
        sema_init(&mutex->mutex, (param->value > 0) ? 1 : 0);
        snprintf(mutex->mutex_name, sizeof(mutex->mutex_name), "%s", param->name);
        mutex->mutex_handle = mutex;
        mutex->mutex_id = MUTEX_ID;

        list_add_tail(&mutex->mutex_list, &drv->m_mutex_list);

    } else {

        dbg_msg( dbg_trace, "%s(): mutex name = %s was found. Use exist mutex\n", __FUNCTION__, param->name );
    }

    atomic_inc(&mutex->mutex_owner_count);
    param->handle = mutex->mutex_handle;

    dbg_msg( dbg_trace, "%s(): %s - mutex_owner_count: %d\n", __FUNCTION__, param->name, atomic_read(&mutex->mutex_owner_count) );

do_out:
    mutex_unlock(&drv->m_mutex_lock);

    return &mutex->mutex_handle;
}

//-----------------------------------------------------------------------------

int ipc_mutex_lock( struct ipc_driver *drv, struct ipc_lock_t *param )
{
    bool exist = false;
    int error = -EINVAL;
    struct ipcmutex_t *entry = NULL;

    dbg_msg( dbg_trace, "%s(): timeout = %d\n", __FUNCTION__, param->timeout );

    if(!drv || !param || !param->handle) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    mutex_lock(&drv->m_mutex_lock);

    entry = container_of(param->handle, struct ipcmutex_t, mutex_handle);
    if(entry && (entry->mutex_id == MUTEX_ID)) {
        exist = true;
    }

    mutex_unlock(&drv->m_mutex_lock);

    if(exist) {

        dbg_msg( dbg_trace, "%s(): %s - mutex_owner_count: %d\n", __FUNCTION__, entry->mutex_name, atomic_read(&entry->mutex_owner_count) );

        if(param->timeout < 0) {
            down(&entry->mutex);
            atomic_inc(&entry->mutex_lock_count);
            error = 0;
        } else {
            error = down_timeout(&entry->mutex, ms_to_jiffies(param->timeout));
            if(error == 0) {
                atomic_inc(&entry->mutex_lock_count);
            }
        }

        dbg_msg( dbg_trace, "%s(): %s - locked %d\n", __FUNCTION__, entry->mutex_name, atomic_read(&entry->mutex_lock_count) );

    } else {

        dbg_msg( dbg_trace, "%s(): Invalid handle\n", __FUNCTION__ );
    }

do_out:
    return error;
}

//-----------------------------------------------------------------------------

int ipc_mutex_unlock( struct ipc_driver *drv, struct ipc_unlock_t *param )
{
    bool exist = false;
    int error = -EINVAL;
    struct ipcmutex_t *entry = NULL;

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    if(!drv || !param || !param->handle) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    mutex_lock(&drv->m_mutex_lock);

    entry = container_of(param->handle, struct ipcmutex_t, mutex_handle);
    if(entry && (entry->mutex_id == MUTEX_ID)) {
        exist = true;
    }

    mutex_unlock(&drv->m_mutex_lock);

    if(exist) {

        dbg_msg( dbg_trace, "%s(): %s - mutex_owner_count: %d\n", __FUNCTION__, entry->mutex_name, atomic_read(&entry->mutex_owner_count) );

        up(&entry->mutex);
        atomic_dec(&entry->mutex_lock_count);
        error = 0;

        dbg_msg( dbg_trace, "%s(): %s - unlocked %d\n", __FUNCTION__, entry->mutex_name, atomic_read(&entry->mutex_lock_count) );

    } else {

        dbg_msg( dbg_trace, "%s(): Invalid handle\n", __FUNCTION__ );
    }

do_out:
    return error;
}

//-----------------------------------------------------------------------------

int ipc_mutex_close( struct ipc_driver *drv, struct ipc_close_t *param )
{
    int error = -EINVAL;
    struct list_head *pos, *n;
    struct ipcmutex_t *entry = NULL;
    struct ipcmutex_t *handle = NULL;

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    if(!drv || !param || !param->handle) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    mutex_lock(&drv->m_mutex_lock);

    handle = container_of(param->handle, struct ipcmutex_t, mutex_handle);

    if(handle && (handle->mutex_id == MUTEX_ID)) {

        list_for_each_safe(pos, n, &drv->m_mutex_list) {

            entry = list_entry(pos, struct ipcmutex_t, mutex_list);

            if(entry == handle) {

                error = 0;

                if(atomic_dec_and_test(&entry->mutex_owner_count)) {

                    dbg_msg( dbg_trace, "%s(): %s - deleted\n", __FUNCTION__, entry->mutex_name );

                    list_del(pos);
                    kfree( (void*)entry );
                    break;

                } else {

                    dbg_msg( dbg_trace, "%s(): %s - mutex is using... skipping to delete it\n", __FUNCTION__, entry->mutex_name );
                }
            }
        }
    }

    mutex_unlock(&drv->m_mutex_lock);

do_out:
    return error;
}

//-----------------------------------------------------------------------------

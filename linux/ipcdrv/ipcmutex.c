
#include <linux/kernel.h>
#define __NO_VERSION__
#include <linux/module.h>
#include <linux/types.h>
#include <linux/ioport.h>
#include <linux/pci.h>
#include <linux/pagemap.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
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

        dbg_msg( dbg_trace, "%s(): mutex name = %s was not found. Create new mutex\n", __FUNCTION__, param->name );

        mutex = (struct ipcmutex_t*)kzalloc(sizeof(struct ipcmutex_t), GFP_KERNEL);
        if(!mutex) {
            err_msg( err_trace, "%s(): Error allocate memory to mutex\n", __FUNCTION__ );
            goto do_out;
        }

        INIT_LIST_HEAD(&mutex->mutex_list);
        mutex->mutex_handle = mutex;
        mutex->mutex_id = MUTEX_ID;
        mutex->m_lockerid = 0;
        atomic_set(&mutex->mutex_owner_count, 0);
        sema_init(&mutex->mutex, 1);

        snprintf(mutex->mutex_name, sizeof(mutex->mutex_name), "%s", param->name);

        if(param->value) {
            dbg_msg( dbg_trace, "%s(%s): Create locked mutex.\n", __FUNCTION__, mutex->mutex_name );
            down_interruptible(&mutex->mutex);
            mutex->m_lockerid = param->lockerid;
            atomic_inc(&mutex->mutex_lock_count);
        } else {
            dbg_msg( dbg_trace, "%s(%s): Create unlocked mutex.\n", __FUNCTION__, mutex->mutex_name );
        }
        list_add_tail(&mutex->mutex_list, &drv->m_mutex_list);

    } else {

        dbg_msg( dbg_trace, "%s(%s): Use exist mutex\n", __FUNCTION__, param->name );
    }

    atomic_inc(&mutex->mutex_owner_count);
    param->handle = mutex->mutex_handle;

    dbg_msg( dbg_trace, "%s(%s): - mutex_owner_count: %d\n", __FUNCTION__, param->name, atomic_read(&mutex->mutex_owner_count) );
    dbg_msg( dbg_trace, "%s(): %s - mutex_handle = %p\n", __FUNCTION__, param->name, mutex->mutex_handle );
    dbg_msg( dbg_trace, "%s(): %s - user_handle = %p\n", __FUNCTION__, param->name, &mutex->mutex_handle );
    dbg_msg( dbg_trace, "%s(): %s - mutex = %p\n", __FUNCTION__, param->name, &mutex->mutex );
    dbg_msg( dbg_trace, "%s(%s): m_lockerid = 0x%x\n", __FUNCTION__, param->name, mutex->m_lockerid );

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

    dbg_msg( dbg_trace, "%s(): param->user_handle = %p\n", __FUNCTION__, param->handle );
    dbg_msg( dbg_trace, "%s(): param->timeout = %d\n", __FUNCTION__, param->timeout );

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

        dbg_msg( dbg_trace, "%s(%s): owner_count: %d, lock_count: %d\n", __FUNCTION__, entry->mutex_name, atomic_read(&entry->mutex_owner_count), atomic_read(&entry->mutex_lock_count) );
        dbg_msg( dbg_trace, "%s(%s): handle = %p\n", __FUNCTION__, entry->mutex_name, &entry->mutex_handle );
        dbg_msg( dbg_trace, "%s(%s): entry = %p\n", __FUNCTION__, entry->mutex_name, entry );
        dbg_msg( dbg_trace, "%s(%s): mutex_id = 0x%x\n", __FUNCTION__, entry->mutex_name, entry->mutex_id );
        dbg_msg( dbg_trace, "%s(%s): lockerid = 0x%x\n", __FUNCTION__, entry->mutex_name, entry->m_lockerid );

        if(entry->m_lockerid == param->lockerid) {
            //atomic_inc(&entry->mutex_lock_count);
            error = 0;
            goto do_out;
        }

        if(param->timeout < 0) {
            dbg_msg( dbg_trace, "%s(%s): try lock - param->lockerid = 0x%x\n", __FUNCTION__, entry->mutex_name, param->lockerid );
            down_interruptible(&entry->mutex);
            atomic_inc(&entry->mutex_lock_count);
            error = 0;
        } else {
            error = down_timeout(&entry->mutex, msecs_to_jiffies(param->timeout));
            if(error == 0) {
                atomic_inc(&entry->mutex_lock_count);
            }
        }

        if(error == 0) {
            entry->m_lockerid = param->lockerid;
            dbg_msg( dbg_trace, "%s(%s): - locked. lock_count: %d, id: 0x%x\n", __FUNCTION__, entry->mutex_name, atomic_read(&entry->mutex_lock_count), entry->m_lockerid);
        }

    } else {

        dbg_msg( err_trace, "%s(): Invalid handle\n", __FUNCTION__ );
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
        if(entry->m_lockerid != param->lockerid)
        {
            error = 0;
            goto do_out;
        }

        dbg_msg( dbg_trace, "%s(%s): owner_count: %d, id - 0x%x\n", __FUNCTION__, entry->mutex_name, atomic_read(&entry->mutex_owner_count), entry->m_lockerid);

        //if(atomic_read(&entry->mutex_lock_count) > 1)
        //{
        //    atomic_dec(&entry->mutex_lock_count);
        //    error = 0;
        //    goto do_out;
        //}

        entry->m_lockerid = 0;
        dbg_msg( dbg_trace, "%s(%s): unlocked %d. id - 0x%x\n", __FUNCTION__, entry->mutex_name, atomic_read(&entry->mutex_lock_count), entry->m_lockerid);
        up(&entry->mutex);
        atomic_dec(&entry->mutex_lock_count);
        error = 0;

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
                    error = -EBUSY;
                }
            }
        }
    }

    if(error == -EBUSY) {
        dbg_msg( dbg_trace, "%s(): %s - mutex is using... skipping to delete it\n", __FUNCTION__, entry->mutex_name );
    }

    mutex_unlock(&drv->m_mutex_lock);

do_out:
    return error;
}

//-----------------------------------------------------------------------------

int ipc_mutex_close_all( struct ipc_driver *drv )
{
    int error = -EINVAL;
    int used_counter = 0;
    struct list_head *pos, *n;
    struct ipcmutex_t *entry = NULL;

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    if(!drv) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    mutex_lock(&drv->m_mutex_lock);

    error = 0;

    list_for_each_safe(pos, n, &drv->m_mutex_list) {

        entry = list_entry(pos, struct ipcmutex_t, mutex_list);

        if(atomic_read(&entry->mutex_owner_count) == 0) {

            dbg_msg( dbg_trace, "%s(): %s - delete\n", __FUNCTION__, entry->mutex_name );

        } else {

            dbg_msg( dbg_trace, "%s(): %s - using. forced deleting\n", __FUNCTION__, entry->mutex_name );
            used_counter++;
        }

        list_del(pos);
        kfree( (void*)entry );
    }

    if(used_counter) {
        dbg_msg( dbg_trace, "%s(%s): used_counter = %d\n", __FUNCTION__, entry->mutex_name, used_counter );
        error = -EBUSY;
    }

    mutex_unlock(&drv->m_mutex_lock);

do_out:
    return error;
}

//-----------------------------------------------------------------------------

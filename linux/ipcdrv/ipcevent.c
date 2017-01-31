
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

void* ipc_event_create( struct ipc_driver *drv, struct ipc_create_t *param )
{
    bool exist = false;
    struct list_head *pos, *n;
    struct ipcevent_t *entry = NULL;
    struct ipcevent_t *event = NULL;

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    if(!drv || !param) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    dbg_msg( dbg_trace, "%s(): name = %s\n", __FUNCTION__, param->name );
    dbg_msg( dbg_trace, "%s(): value = %d\n", __FUNCTION__, param->value );

    mutex_lock(&drv->m_event_lock);

    list_for_each_safe(pos, n, &drv->m_event_list) {

        entry = list_entry(pos, struct ipcevent_t, event_list);

        if(strcmp(entry->event_name, param->name) == 0) {
            event = entry;
            exist = true;
            break;
        }
    }

    if(!exist) {

        dbg_msg( dbg_trace, "%s(): event name = %s was not found. Create new event\n", __FUNCTION__, param->name );

        event = (struct ipcevent_t*)kzalloc(sizeof(struct ipcevent_t), GFP_KERNEL);
        if(!event) {
            err_msg( err_trace, "%s(): Error allocate memory to event\n", __FUNCTION__ );
            goto do_out;
        }

        INIT_LIST_HEAD(&event->event_list);
        sema_init(&event->event, 1);
        snprintf(event->event_name, sizeof(event->event_name), "%s", param->name);
        event->event_handle = event;
        event->event_id = EVENT_ID;
        atomic_set(&event->event_owner_count, 0);

        list_add_tail(&event->event_list, &drv->m_event_list);

    } else {

        dbg_msg( dbg_trace, "%s(): event name = %s was found. Use exist event\n", __FUNCTION__, param->name );
    }

    atomic_inc(&event->event_owner_count);
    param->handle = event->event_handle;

    dbg_msg( dbg_trace, "%s(): %s - event_owner_count: %d\n", __FUNCTION__, param->name, atomic_read(&event->event_owner_count) );

do_out:
    mutex_unlock(&drv->m_event_lock);

    return &event->event_handle;
}

//-----------------------------------------------------------------------------

int ipc_event_lock( struct ipc_driver *drv, struct ipc_lock_t *param )
{
    bool exist = false;
    int error = -EINVAL;
    struct ipcevent_t *entry = NULL;

    dbg_msg( dbg_trace, "%s(): timeout = %d\n", __FUNCTION__, param->timeout );

    if(!drv || !param || !param->handle) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    mutex_lock(&drv->m_event_lock);

    entry = container_of(param->handle, struct ipcevent_t, event_handle);
    if(entry && (entry->event_id == EVENT_ID)) {
        exist = true;
    }

    mutex_unlock(&drv->m_event_lock);

    if(exist) {

        dbg_msg( dbg_trace, "%s(): %s - owner_count: %d, lock_counter: %d\n", __FUNCTION__, entry->event_name,
                 atomic_read(&entry->event_owner_count), atomic_read(&entry->event_lock_count) );

        if(param->timeout < 0) {
            down(&entry->event);
            atomic_inc(&entry->event_lock_count);
            error = 0;
        } else {
            error = down_timeout(&entry->event, msecs_to_jiffies(param->timeout));
            if(error == 0) {
                atomic_inc(&entry->event_lock_count);          
            } else {
               error = -ETIMEDOUT;
            }
        }

    } else {

        dbg_msg( dbg_trace, "%s(): Invalid handle\n", __FUNCTION__ );
    }

do_out:
    return error;
}

//-----------------------------------------------------------------------------

int ipc_event_unlock( struct ipc_driver *drv, struct ipc_unlock_t *param )
{
    bool exist = false;
    int error = -EINVAL;
    struct ipcevent_t *entry = NULL;

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    if(!drv || !param || !param->handle) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    mutex_lock(&drv->m_event_lock);

    entry = container_of(param->handle, struct ipcevent_t, event_handle);
    if(entry && (entry->event_id == EVENT_ID)) {
        exist = true;
    }

    mutex_unlock(&drv->m_event_lock);

    if(exist) {

        dbg_msg( dbg_trace, "%s(): %s - owner_count: %d, lock_counter: %d\n", __FUNCTION__, entry->event_name,
                 atomic_read(&entry->event_owner_count), atomic_read(&entry->event_lock_count) );

        if(atomic_read(&entry->event_lock_count)) {
            up(&entry->event);
            atomic_dec(&entry->event_lock_count);
        }
        error = 0;
        dbg_msg( dbg_trace, "%s(): %s - unlocked %d\n", __FUNCTION__, entry->event_name, atomic_read(&entry->event_lock_count) );

    } else {

        dbg_msg( dbg_trace, "%s(): Invalid handle\n", __FUNCTION__ );
    }

do_out:
    return error;
}

//-----------------------------------------------------------------------------

int ipc_event_close( struct ipc_driver *drv, struct ipc_close_t *param )
{
    int error = -EINVAL;
    struct list_head *pos, *n;
    struct ipcevent_t *entry = NULL;
    struct ipcevent_t *handle = NULL;

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    if(!drv || !param || !param->handle) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    mutex_lock(&drv->m_event_lock);

    handle = container_of(param->handle, struct ipcevent_t, event_handle);

    if(handle && (handle->event_id == EVENT_ID)) {

        list_for_each_safe(pos, n, &drv->m_event_list) {

            entry = list_entry(pos, struct ipcevent_t, event_list);

            if(entry == handle) {

                error = 0;

                if(atomic_dec_and_test(&entry->event_owner_count)) {

                    dbg_msg( dbg_trace, "%s(): %s - deleted\n", __FUNCTION__, entry->event_name );

                    list_del(pos);
                    kfree( (void*)entry );
                    break;

                } else {

                    dbg_msg( dbg_trace, "%s(): %s - event is using... skipping to delete it\n", __FUNCTION__, entry->event_name );
                    error = -EBUSY;
                }
            }
        }
    }

    mutex_unlock(&drv->m_event_lock);

do_out:
    return error;
}

//-----------------------------------------------------------------------------

int ipc_event_reset( struct ipc_driver *drv, struct ipc_reset_t *param )
{
    bool exist = false;
    int error = -EINVAL;
    struct ipcevent_t *entry = NULL;

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    if(!drv || !param || !param->handle) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    mutex_lock(&drv->m_event_lock);

    entry = container_of(param->handle, struct ipcevent_t, event_handle);
    if(entry && (entry->event_id == EVENT_ID)) {
        exist = true;
    }

    mutex_unlock(&drv->m_event_lock);

    if(exist) {

        dbg_msg( dbg_trace, "%s(): %s - owner_count: %d, lock_counter: %d\n", __FUNCTION__, entry->event_name,
                 atomic_read(&entry->event_owner_count), atomic_read(&entry->event_lock_count) );

        if(atomic_read(&entry->event_lock_count)) {
            up(&entry->event);
            atomic_dec(&entry->event_lock_count);
        }
        error = 0;
        dbg_msg( dbg_trace, "%s(): %s - unlocked %d\n", __FUNCTION__, entry->event_name, atomic_read(&entry->event_lock_count) );

    } else {

        dbg_msg( dbg_trace, "%s(): Invalid handle\n", __FUNCTION__ );
    }

do_out:
    return error;
}

//-----------------------------------------------------------------------------

int ipc_event_close_all( struct ipc_driver *drv )
{
    int error = -EINVAL;
    int used_counter = 0;
    struct list_head *pos, *n;
    struct ipcevent_t *entry = NULL;

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    if(!drv) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    mutex_lock(&drv->m_event_lock);

    error = 0;

    list_for_each_safe(pos, n, &drv->m_event_list) {

        entry = list_entry(pos, struct ipcevent_t, event_list);

        if(atomic_read(&entry->event_owner_count) == 0) {

            dbg_msg( dbg_trace, "%s(): %s - delete\n", __FUNCTION__, entry->event_name );

        } else {

            dbg_msg( dbg_trace, "%s(): %s - using. forced deleting\n", __FUNCTION__, entry->event_name );
            used_counter++;
        }

        list_del(pos);
        kfree( (void*)entry );
    }

    if(used_counter)
        error = -EBUSY;

    mutex_unlock(&drv->m_event_lock);

do_out:
    return error;
}

//-----------------------------------------------------------------------------

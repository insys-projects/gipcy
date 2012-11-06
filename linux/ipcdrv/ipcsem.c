
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

void* ipc_sem_create( struct ipc_driver *drv, struct sem_create_t *sem_param )
{
    bool exist = false;
    struct list_head *pos, *n;
    struct ipcsem_t *entry = NULL;
    struct ipcsem_t *sem = NULL;

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    if(!drv || !sem_param) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    dbg_msg( dbg_trace, "%s(): name = %s\n", __FUNCTION__, sem_param->name );
    dbg_msg( dbg_trace, "%s(): value = %d\n", __FUNCTION__, sem_param->value );

    mutex_lock(&drv->m_sem_lock);

    list_for_each_safe(pos, n, &drv->m_sem_list) {

        entry = list_entry(pos, struct ipcsem_t, sem_list);

        if(strcmp(entry->sem_name, sem_param->name) == 0) {
            sem = entry;
            exist = true;
            break;
        }
    }

    if(!exist) {

        dbg_msg( dbg_trace, "%s(): semname = %s was not found. Create new semaphore\n", __FUNCTION__, sem_param->name );

        sem = (struct ipcsem_t*)kzalloc(sizeof(struct ipcsem_t), GFP_KERNEL);
        if(!sem) {
            err_msg( err_trace, "%s(): Error allocate memory to semaphore\n", __FUNCTION__ );
            goto do_out;
        }

        INIT_LIST_HEAD(&sem->sem_list);
        sema_init(&sem->sem, sem_param->value);
        snprintf(sem->sem_name, sizeof(sem->sem_name), "%s", sem_param->name);
        sem->sem_handle = sem;

        list_add_tail(&sem->sem_list, &drv->m_sem_list);

    } else {

        dbg_msg( dbg_trace, "%s(): semname = %s was found. Use exist semaphore\n", __FUNCTION__, sem_param->name );
    }

    atomic_inc(&sem->sem_owner_count);
    sem_param->handle = sem->sem_handle;

    dbg_msg( dbg_trace, "%s(): %s - sem_owner_count: %d\n", __FUNCTION__, sem_param->name, atomic_read(&sem->sem_owner_count) );

do_out:
    mutex_unlock(&drv->m_sem_lock);

    return sem->sem_handle;
}

//-----------------------------------------------------------------------------

int ipc_sem_down( struct ipc_driver *drv, struct sem_lock_t *sem_param )
{
    bool exist = false;
    int error = -EINVAL;
    struct list_head *pos, *n;
    struct ipcsem_t *entry = NULL;

    dbg_msg( dbg_trace, "%s(): timeout = %d\n", __FUNCTION__, sem_param->timeout );

    if(!drv || !sem_param || !sem_param->handle) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    mutex_lock(&drv->m_sem_lock);

    list_for_each_safe(pos, n, &drv->m_sem_list) {

        entry = list_entry(pos, struct ipcsem_t, sem_list);

        if(entry->sem_handle == sem_param->handle) {
            exist = true;
            break;
        }
    }

    mutex_unlock(&drv->m_sem_lock);

    if(exist) {

        dbg_msg( dbg_trace, "%s(): %s - sem_owner_count: %d\n", __FUNCTION__, entry->sem_name, atomic_read(&entry->sem_owner_count) );

        if(sem_param->timeout < 0) {
            down(&entry->sem);
            atomic_inc(&entry->sem_lock_count);
            error = 0;
        } else {
            error = down_timeout(&entry->sem, ms_to_jiffies(sem_param->timeout));
            if(error == 0) {
                atomic_inc(&entry->sem_lock_count);
            }
        }

    } else {

        dbg_msg( dbg_trace, "%s(): Invalid handle\n", __FUNCTION__ );
    }

    dbg_msg( dbg_trace, "%s(): %s - locked %d\n", __FUNCTION__, entry->sem_name, atomic_read(&entry->sem_lock_count) );

do_out:
    return error;
}

//-----------------------------------------------------------------------------

int ipc_sem_up( struct ipc_driver *drv, struct sem_unlock_t *sem_param )
{
    bool exist = false;
    int error = -EINVAL;
    struct list_head *pos, *n;
    struct ipcsem_t *entry = NULL;

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    if(!drv || !sem_param || !sem_param->handle) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    mutex_lock(&drv->m_sem_lock);

    list_for_each_safe(pos, n, &drv->m_sem_list) {

        entry = list_entry(pos, struct ipcsem_t, sem_list);

        if(entry->sem_handle == sem_param->handle) {
            exist = true;
            break;
        }
    }

    mutex_unlock(&drv->m_sem_lock);

    if(exist) {

        dbg_msg( dbg_trace, "%s(): %s - sem_owner_count: %d\n", __FUNCTION__, entry->sem_name, atomic_read(&entry->sem_owner_count) );

        up(&entry->sem);
        atomic_dec(&entry->sem_lock_count);
        error = 0;

    } else {

        dbg_msg( dbg_trace, "%s(): Invalid handle\n", __FUNCTION__ );
    }

    dbg_msg( dbg_trace, "%s(): %s - unlocked %d\n", __FUNCTION__, entry->sem_name, atomic_read(&entry->sem_lock_count) );

do_out:
    return error;
}

//-----------------------------------------------------------------------------

int ipc_sem_close( struct ipc_driver *drv, struct sem_close_t *sem_param )
{
    int error = -EINVAL;
    struct list_head *pos, *n;
    struct ipcsem_t *entry = NULL;

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    if(!drv || !sem_param || !sem_param->handle) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    mutex_lock(&drv->m_sem_lock);

    list_for_each_safe(pos, n, &drv->m_sem_list) {

        entry = list_entry(pos, struct ipcsem_t, sem_list);

        if(entry->sem_handle == sem_param->handle) {

            if(atomic_dec_and_test(&entry->sem_owner_count)) {

                dbg_msg( dbg_trace, "%s(): %s - deleted\n", __FUNCTION__, entry->sem_name );

                list_del(pos);
                kfree( (void*)entry );
                break;

            } else {

                dbg_msg( dbg_trace, "%s(): %s - samaphore is using... skipping to delete it\n", __FUNCTION__, entry->sem_name );
            }
        }
    }

    error = 0;

do_out:
    mutex_unlock(&drv->m_sem_lock);

    return error;
}

//-----------------------------------------------------------------------------

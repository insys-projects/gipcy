
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

void* ipc_shm_open( struct ipc_driver *drv, struct ipc_create_t *param )
{
    bool exist = false;
    struct list_head *pos, *n;
    struct ipcshm_t *entry = NULL;
    struct ipcshm_t *shm = NULL;

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    if(!drv || !param) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    dbg_msg( dbg_trace, "%s(): name = %s\n", __FUNCTION__, param->name );
    dbg_msg( dbg_trace, "%s(): value = %d\n", __FUNCTION__, param->value );

    mutex_lock(&drv->m_shm_lock);

    list_for_each_safe(pos, n, &drv->m_shm_list) {

        entry = list_entry(pos, struct ipcshm_t, shm_list);

        if(strcmp(entry->shm_name, param->name) == 0) {
            shm = entry;
            exist = true;
            break;
        }
    }

    if(!exist) {

        dbg_msg( dbg_trace, "%s(): shared memory name = %s was not found, create new\n", __FUNCTION__, param->name );

        shm = (struct ipcshm_t*)kzalloc(sizeof(struct ipcshm_t), GFP_KERNEL);
        if(!shm) {
            err_msg( err_trace, "%s(): Error allocate memory to shm descriptr\n", __FUNCTION__ );
            goto do_out;
        }

        INIT_LIST_HEAD(&shm->shm_list);
        snprintf(shm->shm_name, sizeof(shm->shm_name), "%s", param->name);
        shm->shm_handle = shm;
        shm->shm_id = SHM_ID;
        atomic_set(&shm->shm_owner_count, 0);

        list_add_tail(&shm->shm_list, &drv->m_shm_list);

    } else {

        dbg_msg( dbg_trace, "%s(): shared memory name = %s was found, use exist\n", __FUNCTION__, param->name );
    }

    atomic_inc(&shm->shm_owner_count);
    param->handle = shm->shm_handle;

    dbg_msg( dbg_trace, "%s(): %s - shm_owner_count: %d\n", __FUNCTION__, param->name, atomic_read(&shm->shm_owner_count) );

do_out:
    mutex_unlock(&drv->m_shm_lock);

    return &shm->shm_handle;
}

//-----------------------------------------------------------------------------

int ipc_shm_close( struct ipc_driver *drv, struct ipc_close_t *param )
{
    int error = -EINVAL;
    struct list_head *pos, *n;
    struct ipcshm_t *entry = NULL;
    struct ipcshm_t *handle = NULL;

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    if(!drv || !param || !param->handle) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    mutex_lock(&drv->m_shm_lock);

    handle = container_of(param->handle, struct ipcshm_t, shm_handle);

    if(handle && (handle->shm_id == SHM_ID)) {

        list_for_each_safe(pos, n, &drv->m_shm_list) {

            entry = list_entry(pos, struct ipcshm_t, shm_list);

            if(entry == handle) {

                error = 0;

                if(atomic_dec_and_test(&entry->shm_owner_count)) {

                    dbg_msg( dbg_trace, "%s(): %s - deleted\n", __FUNCTION__, entry->shm_name );

                    list_del(pos);
                    kfree( (void*)entry );
                    break;

                } else {

                    dbg_msg( dbg_trace, "%s(): %s - shared memory is using... skipping to delete it\n", __FUNCTION__, entry->shm_name );
                    error = -EBUSY;
                }
            }
        }
    }

    mutex_unlock(&drv->m_shm_lock);

do_out:
    return error;
}

//-----------------------------------------------------------------------------

int ipc_shm_close_all( struct ipc_driver *drv )
{
    int error = -EINVAL;
    int used_counter = 0;
    struct list_head *pos, *n;
    struct ipcshm_t *entry = NULL;

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    if(!drv) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    mutex_lock(&drv->m_shm_lock);

    error = 0;

    list_for_each_safe(pos, n, &drv->m_shm_list) {

        entry = list_entry(pos, struct ipcshm_t, shm_list);

        if(atomic_read(&entry->shm_owner_count) == 0) {

            dbg_msg( dbg_trace, "%s(): %s - delete\n", __FUNCTION__, entry->shm_name );

        } else {

            dbg_msg( dbg_trace, "%s(): %s - using. forced deleting\n", __FUNCTION__, entry->shm_name );
            used_counter++;
        }

        list_del(pos);
        kfree( (void*)entry );
    }

    if(used_counter)
        error = -EBUSY;

    mutex_unlock(&drv->m_shm_lock);

do_out:
    return error;
}

//-----------------------------------------------------------------------------

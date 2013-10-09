
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/neutrino.h>
#include <sys/resmgr.h>

#include "ipcdrv.h"
#include "ipcioctl.h"

//-----------------------------------------------------------------------------

void* ipc_shm_open( struct ipc_driver *drv, struct ipc_create_t *param )
{
    bool exist = false;
    struct ipcshm_t *shm = NULL;

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    if(!drv || !param) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    dbg_msg( dbg_trace, "%s(): name = %s\n", __FUNCTION__, param->name );
    dbg_msg( dbg_trace, "%s(): value = %d\n", __FUNCTION__, param->value );

    sem_wait(&drv->m_shm_lock);

    for(unsigned i=0; i<drv->m_shm_list.size(); ++i) {

        struct ipcshm_t *entry = drv->m_shm_list.at(i);

        if(strcmp(entry->shm_name, param->name) == 0) {
            shm = entry;
            exist = true;
            break;
        }
    }

    if(!exist) {

        dbg_msg( dbg_trace, "%s(): shared memory name = %s was not found, create new\n", __FUNCTION__, param->name );

        shm = (struct ipcshm_t*)malloc(sizeof(struct ipcshm_t));
        if(!shm) {
            err_msg( err_trace, "%s(): Error allocate memory to shm descriptr\n", __FUNCTION__ );
            goto do_out;
        }

        memset(shm, 0, sizeof(struct ipcshm_t));

        snprintf(shm->shm_name, sizeof(shm->shm_name), "%s", param->name);
        shm->shm_handle = shm;
        shm->shm_id = SHM_ID;
        shm->shm_owner_count = 0;

        drv->m_shm_list.push_back(shm);

    } else {

        dbg_msg( dbg_trace, "%s(): shared memory name = %s was found, use exist\n", __FUNCTION__, param->name );
    }

    ++shm->shm_owner_count;
    param->handle = shm->shm_handle;

    dbg_msg( dbg_trace, "%s(): %s - shm_owner_count: %d\n", __FUNCTION__, param->name, shm->shm_owner_count );

do_out:
    sem_post(&drv->m_shm_lock);

    return shm->shm_handle;
}

//-----------------------------------------------------------------------------

int ipc_shm_close( struct ipc_driver *drv, struct ipc_close_t *param )
{
    int error = -EINVAL;
    bool exist = false;
    struct ipcshm_t *handle = NULL;

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    if(!drv || !param || !param->handle) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    sem_wait(&drv->m_shm_lock);

    handle = (struct ipcshm_t*)param->handle;
    if(handle) {
    	if(handle->shm_id == MUTEX_ID)
    		exist = true;
    }

    if(exist) {

        for(unsigned i=0; i<drv->m_shm_list.size(); ++i) {

            struct ipcshm_t *entry = drv->m_shm_list.at(i);

            if(entry == handle) {

                error = 0;

                if(--entry->shm_owner_count == 0) {

                    dbg_msg( dbg_trace, "%s(): %s - deleted\n", __FUNCTION__, entry->shm_name );
                    drv->m_shm_list.erase(drv->m_shm_list.begin()+i);
                    free( (void*)entry );
                    break;

                } else {

                    dbg_msg( dbg_trace, "%s(): %s - shared memory is using... skipping to delete it\n", __FUNCTION__, entry->shm_name );
                    error = -EBUSY;
                }
            }
        }
    }

    sem_post(&drv->m_shm_lock);

do_out:
    return error;
}

//-----------------------------------------------------------------------------

int ipc_shm_close_all( struct ipc_driver *drv )
{
    int error = -EINVAL;
    int used_counter = 0;

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    if(!drv) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    sem_wait(&drv->m_shm_lock);

    error = 0;

    for(unsigned i=0; i<drv->m_shm_list.size(); ++i) {

        struct ipcshm_t *entry = drv->m_shm_list.at(i);

        if(entry->shm_owner_count == 0) {

            dbg_msg( dbg_trace, "%s(): %s - delete\n", __FUNCTION__, entry->shm_name );

        } else {

            dbg_msg( dbg_trace, "%s(): %s - using. forced deleting\n", __FUNCTION__, entry->shm_name );
            used_counter++;
        }

        drv->m_shm_list.erase(drv->m_shm_list.begin()+i);
        free( (void*)entry );
    }

    if(used_counter)
        error = -EBUSY;

    sem_post(&drv->m_shm_lock);

do_out:
    return error;
}

//-----------------------------------------------------------------------------

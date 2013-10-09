
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

void* ipc_sem_create( struct ipc_driver *drv, struct ipc_create_t *param )
{
    bool exist = false;
    struct ipcsem_t *entry = NULL;
    struct ipcsem_t *sem = NULL;

    sem_wait(&drv->m_sem_lock);

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    if(!drv || !param) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    dbg_msg( dbg_trace, "%s(): name = %s\n", __FUNCTION__, param->name );
    dbg_msg( dbg_trace, "%s(): value = %d\n", __FUNCTION__, param->value );

    for(unsigned i=0; i<drv->m_sem_list.size(); i++) {

        entry = drv->m_sem_list.at(i);

        if(strcmp(entry->sem_name, param->name) == 0) {
            sem = entry;
            exist = true;
            break;
        }
    }

    if(!exist) {

        dbg_msg( dbg_trace, "%s(): semaphore name = %s was not found. Create new semaphore\n", __FUNCTION__, param->name );

        sem = (struct ipcsem_t*)malloc(sizeof(struct ipcsem_t));
        if(!sem) {
            err_msg( err_trace, "%s(): Error allocate memory to semaphore\n", __FUNCTION__ );
            goto do_out;
        }

        memset(sem, 0, sizeof(struct ipcsem_t));

        sem_init(&sem->sem, 0, param->value);
        snprintf(sem->sem_name, sizeof(sem->sem_name), "%s", param->name);
        sem->sem_handle = sem;
        sem->sem_id = SEM_ID;
        sem->sem_owner_count = 0;

        drv->m_sem_list.push_back(sem);

    } else {

        dbg_msg( dbg_trace, "%s(): semaphore name = %s was found. Use exist semaphore\n", __FUNCTION__, param->name );
    }

    ++sem->sem_owner_count;
    param->handle = sem->sem_handle;

    dbg_msg( dbg_trace, "%s(): %s - sem_owner_count: %d\n", __FUNCTION__, param->name, sem->sem_owner_count );

do_out:
    sem_post(&drv->m_sem_lock);

    return sem->sem_handle;
}

//-----------------------------------------------------------------------------

int ipc_sem_lock( struct ipc_driver *drv, struct ipc_lock_t *param )
{
    bool exist = false;
    int error = -EINVAL;
    struct ipcsem_t *entry = NULL;

    dbg_msg( dbg_trace, "%s(): timeout = %d\n", __FUNCTION__, param->timeout );

    if(!drv || !param || !param->handle) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    sem_wait(&drv->m_sem_lock);

    entry = (struct ipcsem_t*)param->handle;
    if(entry) {
    	if(entry->sem_id == SEM_ID)
    		exist = true;
    }

    sem_post(&drv->m_sem_lock);

    if(exist) {

        dbg_msg( dbg_trace, "%s(): %s - sem_owner_count: %d\n", __FUNCTION__, entry->sem_name, entry->sem_owner_count );

        if(param->timeout < 0) {

            sem_wait(&entry->sem);
            ++entry->sem_lock_count;
            error = 0;

        } else {

        	struct timespec tm = ms_to_timespec(param->timeout);

        	error = sem_timedwait(&entry->sem, &tm);
            if(error == 0) {
                ++entry->sem_lock_count;
                dbg_msg( dbg_trace, "%s(): %s - locked %d\n", __FUNCTION__, entry->sem_name, entry->sem_lock_count );
            } else {
            	dbg_msg( dbg_trace, "%s(): %s\n", __FUNCTION__, strerror(errno) );
            	error = errno;
            }
        }

    } else {

        dbg_msg( dbg_trace, "%s(): Invalid handle\n", __FUNCTION__ );
    }

do_out:
    return error;
}

//-----------------------------------------------------------------------------

int ipc_sem_unlock( struct ipc_driver *drv, struct ipc_unlock_t *param )
{
    bool exist = false;
    int error = -EINVAL;
    struct ipcsem_t *entry = NULL;

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    if(!drv || !param || !param->handle) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    sem_wait(&drv->m_sem_lock);

    entry = (struct ipcsem_t*)param->handle;
    if(entry) {
    	if(entry->sem_id == SEM_ID)
    		exist = true;
    }

    sem_post(&drv->m_sem_lock);

    if(exist) {

        dbg_msg( dbg_trace, "%s(): %s - sem_owner_count: %d\n", __FUNCTION__, entry->sem_name, entry->sem_owner_count );

        sem_post(&entry->sem);
        --entry->sem_lock_count;
        error = 0;
        dbg_msg( dbg_trace, "%s(): %s - unlocked %d\n", __FUNCTION__, entry->sem_name, entry->sem_lock_count );

    } else {

        dbg_msg( dbg_trace, "%s(): Invalid handle\n", __FUNCTION__ );
    }

do_out:
    return error;
}

//-----------------------------------------------------------------------------

int ipc_sem_close( struct ipc_driver *drv, struct ipc_close_t *param )
{
    int error = -EINVAL;
    bool exist = false;
    struct ipcsem_t *handle = NULL;

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    if(!drv || !param || !param->handle) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    sem_post(&drv->m_sem_lock);

    handle = (struct ipcsem_t*)param->handle;
    if(handle) {
    	if(handle->sem_id == SEM_ID)
    		exist = true;
    }

    if(exist) {

        for(unsigned i=0; i<drv->m_sem_list.size(); ++i) {

            struct ipcsem_t *entry = drv->m_sem_list.at(i);

            if(entry == handle) {

                error = 0;
                dbg_msg( dbg_trace, "%s(): %s - found\n", __FUNCTION__, entry->sem_name );

                if((--entry->sem_owner_count) == 0) {

                    sem_destroy(&entry->sem);

                    dbg_msg( dbg_trace, "%s(): %s - deleted\n", __FUNCTION__, entry->sem_name );
                    drv->m_sem_list.erase(drv->m_sem_list.begin()+i);
                    free( (void*)entry );
                    break;

                } else {

                    dbg_msg( dbg_trace, "%s(): %s - semaphore is using... skipping to delete it\n", __FUNCTION__, entry->sem_name );
                    error = -EBUSY;
                }
            }
        }
    }

    sem_post(&drv->m_sem_lock);

do_out:
    return error;
}

//-----------------------------------------------------------------------------

int ipc_sem_close_all( struct ipc_driver *drv )
{
    int error = -EINVAL;
    int used_counter = 0;

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    if(!drv) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    sem_wait(&drv->m_sem_lock);

    error = 0;

    for(unsigned i=0; i<drv->m_sem_list.size(); ++i) {

        struct ipcsem_t *entry = drv->m_sem_list.at(i);

        if(entry->sem_owner_count == 0) {

        	sem_destroy(&entry->sem);
            dbg_msg( dbg_trace, "%s(): %s - delete\n", __FUNCTION__, entry->sem_name );

        } else {

            dbg_msg( dbg_trace, "%s(): %s - using. forced deleting\n", __FUNCTION__, entry->sem_name );
            used_counter++;
        }

        drv->m_sem_list.erase(drv->m_sem_list.begin()+i);
        free( (void*)entry );
    }

    if(used_counter)
        error = -EBUSY;

    sem_post(&drv->m_sem_lock);

do_out:
    return error;
}

//-----------------------------------------------------------------------------

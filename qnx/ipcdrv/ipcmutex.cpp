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

void* ipc_mutex_create( struct ipc_driver *drv, struct ipc_create_t *param )
{
    bool exist = false;
    struct ipcmutex_t *entry = NULL;
    struct ipcmutex_t *mutex = NULL;

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    if(!drv || !param) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    dbg_msg( dbg_trace, "%s(): name = %s\n", __FUNCTION__, param->name );
    dbg_msg( dbg_trace, "%s(): value = %d\n", __FUNCTION__, param->value );

    sem_wait(&drv->m_mutex_lock);

    for(unsigned i=0; i<drv->m_mutex_list.size(); ++i) {

        entry = drv->m_mutex_list.at(i);

        if(strcmp(entry->mutex_name, param->name) == 0) {
        	mutex = entry;
            exist = true;
            break;
        }
    }

    if(!exist) {

        dbg_msg( dbg_trace, "%s(): mutex name = %s was not found. Create new mutex\n", __FUNCTION__, param->name );

        mutex = (struct ipcmutex_t*)malloc(sizeof(struct ipcmutex_t));
        if(!mutex) {
            err_msg( err_trace, "%s(): Error allocate memory to mutex\n", __FUNCTION__ );
            goto do_out;
        }

        memset(mutex, 0, sizeof(struct ipcmutex_t));
        if(param->value) {
            sem_init(&mutex->mutex, 0, param->value);
            dbg_msg( dbg_trace, "%s(): param->value = %d\n", __FUNCTION__, param->value );
        } else {
            sem_init(&mutex->mutex, 0, 1);
            sem_wait(&mutex->mutex);
            dbg_msg( dbg_trace, "%s(): param->value = %d\n", __FUNCTION__, param->value );
        }
        snprintf(mutex->mutex_name, sizeof(mutex->mutex_name), "%s", param->name);
        mutex->mutex_handle = mutex;
        mutex->mutex_id = MUTEX_ID;
        mutex->mutex_owner_count = 0;

        drv->m_mutex_list.push_back(mutex);

    } else {

        dbg_msg( dbg_trace, "%s(): mutex name = %s was found. Use exist mutex\n", __FUNCTION__, param->name );
    }

    ++mutex->mutex_owner_count;
    param->handle = mutex->mutex_handle;

    dbg_msg( dbg_trace, "%s(): %s - mutex_owner_count: %d\n", __FUNCTION__, param->name, mutex->mutex_owner_count );
    dbg_msg( dbg_trace, "%s(): %s - mutex_handle = %p\n", __FUNCTION__, param->name, mutex->mutex_handle );
    dbg_msg( dbg_trace, "%s(): %s - user_handle = %p\n", __FUNCTION__, param->name, &mutex->mutex_handle );
    dbg_msg( dbg_trace, "%s(): %s - mutex = %p\n", __FUNCTION__, param->name, &mutex->mutex );

do_out:
    sem_post(&drv->m_mutex_lock);

    return mutex->mutex_handle;
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

    sem_wait(&drv->m_mutex_lock);

    entry = (struct ipcmutex_t*)param->handle;
    if(entry) {
    	if(entry->mutex_id == MUTEX_ID)
    		exist = true;
    }

    sem_post(&drv->m_mutex_lock);

    if(exist) {

        dbg_msg( dbg_trace, "%s(): %s - mutex_owner_count: %d\n", __FUNCTION__, entry->mutex_name, entry->mutex_owner_count );
        dbg_msg( dbg_trace, "%s(): %s - mutex_handle = %p\n", __FUNCTION__, entry->mutex_name, &entry->mutex_handle );
        dbg_msg( dbg_trace, "%s(): entry = %p\n", __FUNCTION__, entry );
        dbg_msg( dbg_trace, "%s(): entry->mutex_id = 0x%x\n", __FUNCTION__, entry->mutex_id );

        if(param->timeout < 0) {
            sem_wait(&entry->mutex);
            ++entry->mutex_lock_count;
            error = 0;
        } else {

        	struct timespec tm = ms_to_timespec(param->timeout);

        	error = sem_timedwait(&entry->mutex, &tm);
            if(error == 0) {
                ++entry->mutex_lock_count;
            }
        }

        dbg_msg( dbg_trace, "%s(): %s - locked %d\n", __FUNCTION__, entry->mutex_name, entry->mutex_lock_count );

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

    sem_wait(&drv->m_mutex_lock);

    entry = (struct ipcmutex_t*)param->handle;
    if(entry) {
    	if(entry->mutex_id == MUTEX_ID)
    		exist = true;
    }

    sem_post(&drv->m_mutex_lock);

    if(exist) {

        dbg_msg( dbg_trace, "%s(): %s - mutex_owner_count: %d\n", __FUNCTION__, entry->mutex_name, entry->mutex_owner_count );

        sem_post(&entry->mutex);
        --entry->mutex_lock_count;
        error = 0;
        dbg_msg( dbg_trace, "%s(): %s - unlocked %d\n", __FUNCTION__, entry->mutex_name, entry->mutex_lock_count );

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
    bool exist = true;
    struct ipcmutex_t *handle = NULL;

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    if(!drv || !param || !param->handle) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    sem_wait(&drv->m_mutex_lock);

    handle = (struct ipcmutex_t*)param->handle;
    if(handle) {
    	if(handle->mutex_id == MUTEX_ID)
    		exist = true;
    }

    if(exist) {

        for(unsigned i=0; i<drv->m_mutex_list.size(); ++i) {

            struct ipcmutex_t *entry = drv->m_mutex_list.at(i);

            if(entry == handle) {

                error = 0;

                if(--entry->mutex_owner_count == 0) {

                	sem_destroy(&entry->mutex);
                    dbg_msg( dbg_trace, "%s(): %s - deleted\n", __FUNCTION__, entry->mutex_name );
                    drv->m_mutex_list.erase(drv->m_mutex_list.begin()+i);
                    free( (void*)entry );
                    break;

                } else {

                    dbg_msg( dbg_trace, "%s(): %s - mutex is using... skipping to delete it\n", __FUNCTION__, entry->mutex_name );
                    error = -EBUSY;
                }
            }
        }
    }

    sem_post(&drv->m_mutex_lock);

do_out:
    return error;
}

//-----------------------------------------------------------------------------

int ipc_mutex_close_all( struct ipc_driver *drv )
{
    int error = -EINVAL;
    int used_counter = 0;

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    if(!drv) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    sem_wait(&drv->m_mutex_lock);

    error = 0;

    for(unsigned i=0; i<drv->m_mutex_list.size(); ++i) {

        struct ipcmutex_t *entry = drv->m_mutex_list.at(i);

        if(entry->mutex_owner_count == 0) {

        	sem_destroy(&entry->mutex);
            dbg_msg( dbg_trace, "%s(): %s - delete\n", __FUNCTION__, entry->mutex_name );

        } else {

            dbg_msg( dbg_trace, "%s(): %s - using. forced deleting\n", __FUNCTION__, entry->mutex_name );
            used_counter++;
        }

        drv->m_mutex_list.erase(drv->m_mutex_list.begin()+i);
        free( (void*)entry );
    }

    if(used_counter)
        error = -EBUSY;

    sem_post(&drv->m_mutex_lock);

do_out:
    return error;
}

//-----------------------------------------------------------------------------

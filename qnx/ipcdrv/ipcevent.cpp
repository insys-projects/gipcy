
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

void* ipc_event_create( struct ipc_driver *drv, struct ipc_create_t *param )
{
    bool exist = false;
    struct ipcevent_t *entry = NULL;
    struct ipcevent_t *event = NULL;

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    if(!drv || !param) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    dbg_msg( dbg_trace, "%s(): name = %s\n", __FUNCTION__, param->name );
    dbg_msg( dbg_trace, "%s(): value = %d\n", __FUNCTION__, param->value );

    sem_wait(&drv->m_event_lock);

    for(unsigned i=0; i<drv->m_event_list.size(); ++i) {

        entry = drv->m_event_list.at(i);

        if(strcmp(entry->event_name, param->name) == 0) {
        	event = entry;
            exist = true;
            break;
        }
    }

    if(!exist) {

        dbg_msg( dbg_trace, "%s(): event name = %s was not found. Create new event\n", __FUNCTION__, param->name );

        event = (struct ipcevent_t*)malloc(sizeof(struct ipcevent_t));
        if(!event) {
            err_msg( err_trace, "%s(): Error allocate memory to event\n", __FUNCTION__ );
            goto do_out;
        }

        memset(event, 0, sizeof(struct ipcevent_t));

        sem_init(&event->event, 0, 1);
        snprintf(event->event_name, sizeof(event->event_name), "%s", param->name);
        event->event_handle = event;
        event->event_id = EVENT_ID;
        event->event_owner_count = 0;

        drv->m_event_list.push_back(event);

    } else {

        dbg_msg( dbg_trace, "%s(): event name = %s was found. Use exist event\n", __FUNCTION__, param->name );
    }

    ++event->event_owner_count;
    param->handle = event->event_handle;

    dbg_msg( dbg_trace, "%s(): %s - event_owner_count: %d\n", __FUNCTION__, param->name, event->event_owner_count );

do_out:
    sem_post(&drv->m_event_lock);

    return event->event_handle;
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

    sem_wait(&drv->m_event_lock);

    entry = (struct ipcevent_t*)param->handle;
    if(entry) {
    	if(entry->event_id == EVENT_ID)
    		exist = true;
    }

    sem_post(&drv->m_event_lock);

    if(exist) {

        dbg_msg( dbg_trace, "%s(): %s - event_owner_count: %d\n", __FUNCTION__, entry->event_name, entry->event_owner_count );

        if(param->timeout < 0) {
            sem_wait(&entry->event);
            ++entry->event_lock_count;
            error = 0;
        } else {

        	struct timespec tm = ms_to_timespec(param->timeout);

            error = sem_timedwait(&entry->event, &tm);
            if(error == 0) {
                ++entry->event_lock_count;
            }
        }

        dbg_msg( dbg_trace, "%s(): %s - locked %d\n", __FUNCTION__, entry->event_name, entry->event_lock_count );

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

    sem_wait(&drv->m_event_lock);

    entry = (struct ipcevent_t*)param->handle;
    if(entry) {
    	if(entry->event_id == EVENT_ID)
    		exist = true;
    }

    sem_post(&drv->m_event_lock);

    if(exist) {

        dbg_msg( dbg_trace, "%s(): %s - event_owner_count: %d\n", __FUNCTION__, entry->event_name, entry->event_owner_count );

        sem_post(&entry->event);
        --entry->event_lock_count;
        error = 0;
        dbg_msg( dbg_trace, "%s(): %s - unlocked %d\n", __FUNCTION__, entry->event_name, entry->event_lock_count );

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
    bool exist = false;
    struct ipcevent_t *handle = NULL;

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    if(!drv || !param || !param->handle) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    sem_wait(&drv->m_event_lock);

    handle = (struct ipcevent_t*)param->handle;
    if(handle) {
    	if(handle->event_id == MUTEX_ID)
    		exist = true;
    }

    if(exist) {

        for(unsigned i=0; i<drv->m_event_list.size(); ++i) {

            struct ipcevent_t *entry = drv->m_event_list.at(i);

            if(entry == handle) {

                error = 0;

                if(--entry->event_owner_count == 0) {

                	sem_destroy(&entry->event);
                    dbg_msg( dbg_trace, "%s(): %s - deleted\n", __FUNCTION__, entry->event_name );
                    drv->m_event_list.erase(drv->m_event_list.begin()+i);
                    free( (void*)entry );
                    break;

                } else {

                    dbg_msg( dbg_trace, "%s(): %s - event is using... skipping to delete it\n", __FUNCTION__, entry->event_name );
                    error = -EBUSY;
                }
            }
        }
    }

    sem_post(&drv->m_event_lock);

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

    sem_wait(&drv->m_event_lock);

    entry = (struct ipcevent_t*)param->handle;
    if(entry) {
    	if(entry->event_id == MUTEX_ID)
    		exist = true;
    }

    sem_post(&drv->m_event_lock);

    if(exist) {

        dbg_msg( dbg_trace, "%s(): %s - event_owner_count: %d\n", __FUNCTION__, entry->event_name, entry->event_owner_count );

        sem_post(&entry->event);
        --entry->event_lock_count;
        error = 0;
        dbg_msg( dbg_trace, "%s(): %s - unlocked %d\n", __FUNCTION__, entry->event_name, entry->event_lock_count );

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

    dbg_msg( dbg_trace, "%s()\n", __FUNCTION__ );

    if(!drv) {
        err_msg( err_trace, "%s(): Invalid parameters\n", __FUNCTION__ );
        goto do_out;
    }

    sem_wait(&drv->m_event_lock);

    error = 0;

    for(unsigned i=0; i<drv->m_event_list.size(); ++i) {

        struct ipcevent_t *entry = drv->m_event_list.at(i);

        if(entry->event_owner_count == 0) {

        	sem_destroy(&entry->event);
            dbg_msg( dbg_trace, "%s(): %s - delete\n", __FUNCTION__, entry->event_name );

        } else {

            dbg_msg( dbg_trace, "%s(): %s - using. forced deleting\n", __FUNCTION__, entry->event_name );
            used_counter++;
        }

        drv->m_event_list.erase(drv->m_event_list.begin()+i);
        free( (void*)entry );
    }

    if(used_counter)
        error = -EBUSY;

    sem_post(&drv->m_event_lock);

do_out:
    return error;
}

//-----------------------------------------------------------------------------

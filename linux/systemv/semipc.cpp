
#ifdef __IPC_LINUX__

#ifndef __LINIPC_H__
#include "linipc.h"
#endif
#ifndef __SEMIPC_H__
#include "semipc.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <error.h>
#include <time.h>

//-----------------------------------------------------------------------------

IPC_handle IPC_createSemaphore(const IPC_str *name, int value)
{
    ipc_handle_t h = allocate_ipc_object(name, IPC_typeSemaphore);
    if(!h)
        return NULL;

    h->ipc_data = NULL;

    union semun semarg;
    struct semid_ds seminfo;

    h->ipc_descr.ipc_sem = semget(h->ipc_key, 1, IPC_SVSEM_MODE | IPC_CREAT | IPC_EXCL);

    if(h->ipc_descr.ipc_sem > 0) {

        semarg.val = value;

        int res = semctl(h->ipc_descr.ipc_sem, 0, SETVAL, semarg);
        if(res < 0) {
            DEBUG_PRINT("%s(): semaphore - %s created but not initialized\n", __FUNCTION__, h->ipc_name);
            delete_ipc_object(h);
            return NULL;
        }

    } else if(errno == EEXIST) {

        h->ipc_descr.ipc_sem = semget(h->ipc_key, 1, IPC_SVSEM_MODE);

        semarg.buf = &seminfo;

        for(int i=0; i<10; i++) {

            int res = semctl(h->ipc_descr.ipc_sem, 0, IPC_STAT, semarg);
            if(res < 0) continue;
            if(semarg.buf->sem_otime != 0) {
                DEBUG_PRINT("%s(): semaphore - %s opened\n", __FUNCTION__, h->ipc_name);
                return h;
            }
            usleep(100);
        }

        DEBUG_PRINT("%s(): semaphore - %s created but not initialized\n", __FUNCTION__, h->ipc_name);
        delete_ipc_object(h);
        return NULL;
    }

    DEBUG_PRINT("%s(): semaphore - %s created\n", __FUNCTION__, h->ipc_name);

    return h;
}

//-----------------------------------------------------------------------------

int IPC_lockSemaphore(const  IPC_handle handle, int timeout)
{
    if(!handle)
        return IPC_invalidHandle;

    ipc_handle_t h = (ipc_handle_t)handle;

    struct sembuf ops;

    ops.sem_num = 0;
    ops.sem_op = -1;
    ops.sem_flg = SEM_UNDO;

    if(timeout > 0) {

        struct timespec ts;

        int nsec = timeout / 1000;

        ts.tv_sec = nsec;
        ts.tv_nsec = timeout;

        //DEBUG_PRINT("%s(): ts.tv_sec = %d\n", __FUNCTION__, (int)ts.tv_sec);
        //DEBUG_PRINT("%s(): ts.tv_nsec = %d\n", __FUNCTION__, (int)ts.tv_nsec);
        //DEBUG_PRINT("%s(): Try lock semaphore - %s\n", __FUNCTION__, h->ipc_name);

        int res = semtimedop(h->ipc_descr.ipc_sem, &ops, 1, &ts);
        if(res < 0) {
            if(errno == EINTR) {
                DEBUG_PRINT("%s(): Waiting was interrputed - %s\n", __FUNCTION__, h->ipc_name);
                return IPC_interrupted;
            }
            if(errno == EAGAIN) {
                DEBUG_PRINT("%s(): Specified timeout expired - %s\n", __FUNCTION__, h->ipc_name);
                return IPC_timeout;
            }
            DEBUG_PRINT("%s(): %s - %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
            return IPC_generalError;
        }

    } else {

        int res = semtimedop(h->ipc_descr.ipc_sem, &ops, 1, NULL);
        if(res < 0) {
            if(errno == EINTR) {
                DEBUG_PRINT("%s(): Waiting was interrputed - %s\n", __FUNCTION__, h->ipc_name);
                return IPC_interrupted;
            }
            DEBUG_PRINT("%s(): %s - %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
            return IPC_generalError;
        }
    }

    //DEBUG_PRINT("%s(): semaphore - %s locked\n", __FUNCTION__, h->ipc_name);

    return IPC_ok;
}

//-----------------------------------------------------------------------------

int IPC_unlockSemaphore(const  IPC_handle handle)
{
    if(!handle)
        return IPC_invalidHandle;

    ipc_handle_t h = (ipc_handle_t)handle;

    struct sembuf ops;

    ops.sem_num = 0;
    ops.sem_op = 1;
    ops.sem_flg = SEM_UNDO;

    int res = semop(h->ipc_descr.ipc_sem,&ops,1);
    if(res < 0) {
        if(errno == EINTR) {
            DEBUG_PRINT("%s(): Posting was interrputed - %s\n", __FUNCTION__, h->ipc_name);
            return IPC_interrupted;
        }
        DEBUG_PRINT("%s(): %s - %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
        return IPC_generalError;
    }

    //DEBUG_PRINT("%s(): semaphore - %s unlocked\n", __FUNCTION__, h->ipc_name);

    return IPC_ok;
}

//-----------------------------------------------------------------------------
/*
static bool is_ok_remove(IPC_handle handle)
{
    if(!handle)
        return false;

    ipc_handle_t h = (ipc_handle_t)handle;

    if(h->ipc_type != IPC_typeSemaphore)
        return false;

    int semid = h->ipc_descr.ipc_sem;
#ifdef __VERBOSE__
    char *fname = h->ipc_name;
#endif
    union semun arg = {0};
    int semncnt, semzcnt, semval;

    if( (semval = semctl(semid, 0, GETVAL, arg)) == -1 ) {
        DEBUG_PRINT( "%s(): semctl(getval) error %s\n", __FUNCTION__, fname );
        DEBUG_PRINT( "%s(): %s\n", __FUNCTION__, strerror(errno) );
        return false;
    }

    DEBUG_PRINT( "%s(): %s - semval = %d\n", __FUNCTION__, fname, semval );

    if( (semncnt = semctl(semid, 0, GETNCNT, arg)) == -1 ) {
        DEBUG_PRINT( "%s(): semctl(getncnt) error %s\n", __FUNCTION__, fname );
        DEBUG_PRINT( "%s(): %s\n", __FUNCTION__, strerror(errno) );
        return false;
    }

    DEBUG_PRINT( "%s(): %s - semncnt = %d\n", __FUNCTION__, fname, semncnt );

    if( (semzcnt = semctl(semid, 0, GETZCNT, arg)) == -1 ) {
        DEBUG_PRINT( "%s(): semctl(getzcnt) error %s\n", __FUNCTION__, fname );
        DEBUG_PRINT( "%s(): %s\n", __FUNCTION__, strerror(errno) );
        return false;
    }

    DEBUG_PRINT( "%s(): %s - semzcnt = %d\n", __FUNCTION__, fname, semzcnt );

    if( (semncnt == 0) && (semzcnt == 0) && (semval == 1) ) {
        return true;
    }

    return false;
}
*/
//-----------------------------------------------------------------------------

int IPC_deleteSemaphore(IPC_handle handle)
{
    if(!handle)
        return IPC_invalidHandle;

    ipc_handle_t h = (ipc_handle_t)handle;

    if(h->ipc_type != IPC_typeSemaphore)
        return IPC_invalidHandle;

    if( is_ok_remove(h) ) {

        if( semctl(h->ipc_descr.ipc_sem, 0, IPC_RMID) < 0 ) {
            DEBUG_PRINT( "%s(): semctl(ipc_rmid) error %s\n", __FUNCTION__, h->ipc_name );
            DEBUG_PRINT( "%s(): %s\n", __FUNCTION__, strerror(errno) );
            return IPC_generalError;
        }
    }

    DEBUG_PRINT("%s(): semaphore - %s deleted\n", __FUNCTION__, h->ipc_name);

    delete_ipc_object((ipc_handle_t)handle);

    return IPC_ok;
}

//-----------------------------------------------------------------------------

#endif //__IPC_LINUX__

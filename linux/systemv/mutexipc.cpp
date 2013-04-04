
#ifdef __IPC_LINUX__
#ifndef __LINIPC_H__
    #include "linipc.h"
#endif
#ifndef __GIPCY_H__
    #include "gipcy.h"
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

IPC_handle IPC_createMutex(const IPC_str *name, bool value)
{
    ipc_handle_t h = allocate_ipc_object(name, IPC_typeMutex);
    if(!h)
        return NULL;

    h->ipc_data = NULL;

    union semun semarg;
    struct semid_ds seminfo;

    h->ipc_descr.ipc_sem = semget(h->ipc_key, 1, IPC_SVSEM_MODE | IPC_CREAT | IPC_EXCL);

    if(h->ipc_descr.ipc_sem > 0) {

        semarg.val = 1;

        int res = semctl(h->ipc_descr.ipc_sem, 0, SETVAL, semarg);
        if(res < 0) {
            DEBUG_PRINT("%s(): mutex - %s created but not initialized\n", __FUNCTION__, h->ipc_name);
            delete_ipc_object(h);
            return NULL;
        }

    } else if(errno == EEXIST) {

        h->ipc_descr.ipc_sem = semget(h->ipc_key, 1, IPC_SVSEM_MODE);

        semarg.buf = &seminfo;

        for(int i=0; i<10; i++) {

            int res = semctl(h->ipc_descr.ipc_sem, 0, IPC_STAT, semarg);
            if(res < 0) continue;
            if(semarg.buf->sem_ctime != 0) {
                DEBUG_PRINT("%s(): mutex - %s opened\n", __FUNCTION__, h->ipc_name);
                return h;
            }
            usleep(100);
        }

        DEBUG_PRINT("%s(): mutex - %s created but not initialized\n", __FUNCTION__, h->ipc_name);
        delete_ipc_object(h);
        return NULL;
    }

    DEBUG_PRINT("%s(): mutex - %s created\n", __FUNCTION__, h->ipc_name);

    return h;
}

//-----------------------------------------------------------------------------

int IPC_captureMutex(const  IPC_handle handle, int timeout)
{
    if(!handle)
        return IPC_INVALID_HANDLE;

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
        //DEBUG_PRINT("%s(): Try lock mutex - %s\n", __FUNCTION__, h->ipc_name);

#ifndef DZYTOOLS_2_4_X
        int res = semtimedop(h->ipc_descr.ipc_sem, &ops, 1, &ts);
#else
        int res = semop(h->ipc_descr.ipc_sem, &ops, 1);
#endif
        if(res < 0) {
            if(errno == EINTR) {
                DEBUG_PRINT("%s(): Waiting was interrputed - %s\n", __FUNCTION__, h->ipc_name);
                return IPC_GENERAL_ERROR;
            }
            if(errno == EAGAIN) {
                DEBUG_PRINT("%s(): Specified timeout expired - %s\n", __FUNCTION__, h->ipc_name);
                return IPC_WAIT_TIMEOUT;
            }
            DEBUG_PRINT("%s(): %s - %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
            return IPC_GENERAL_ERROR;
        }

    } else {

#ifndef DZYTOOLS_2_4_X
        int res = semtimedop(h->ipc_descr.ipc_sem, &ops, 1, NULL);
#else
        int res = semop(h->ipc_descr.ipc_sem, &ops, 1);
#endif
        if(res < 0) {
            if(errno == EINTR) {
                DEBUG_PRINT("%s(): Waiting was interrputed - %s\n", __FUNCTION__, h->ipc_name);
                return IPC_GENERAL_ERROR;
            }
            DEBUG_PRINT("%s(): %s - %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
            return IPC_GENERAL_ERROR;
        }
    }

    DEBUG_PRINT("%s(): mutex - %s locked\n", __FUNCTION__, h->ipc_name);

    return IPC_OK;
}

//-----------------------------------------------------------------------------

int IPC_releaseMutex(const  IPC_handle handle)
{
    if(!handle)
        return IPC_INVALID_HANDLE;

    ipc_handle_t h = (ipc_handle_t)handle;

    struct sembuf ops;

    ops.sem_num = 0;
    ops.sem_op = 1;
    ops.sem_flg = SEM_UNDO;

    int res = semop(h->ipc_descr.ipc_sem,&ops,1);
    if(res < 0) {
        if(errno == EINTR) {
            DEBUG_PRINT("%s(): Posting was interrputed - %s\n", __FUNCTION__, h->ipc_name);
            return IPC_GENERAL_ERROR;
        }
        DEBUG_PRINT("%s(): %s - %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
        return IPC_GENERAL_ERROR;
    }

    DEBUG_PRINT("%s(): mutex - %s unlocked\n", __FUNCTION__, h->ipc_name);

    return IPC_OK;
}

//-----------------------------------------------------------------------------

int IPC_deleteMutex(IPC_handle handle)
{
    if(!handle)
        return IPC_INVALID_HANDLE;

    ipc_handle_t h = (ipc_handle_t)handle;

    if(h->ipc_type != IPC_typeMutex)
        return IPC_INVALID_HANDLE;

    if( is_ok_remove(h) ) {

        if( semctl(h->ipc_descr.ipc_sem, 0, IPC_RMID) < 0 ) {
            DEBUG_PRINT( "%s(): semctl(ipc_rmid) error %s\n", __FUNCTION__, h->ipc_name );
            DEBUG_PRINT( "%s(): %s\n", __FUNCTION__, strerror(errno) );
            return IPC_GENERAL_ERROR;
        }
    }

    DEBUG_PRINT("%s(): mutex - %s deleted\n", __FUNCTION__, h->ipc_name);

    delete_ipc_object((ipc_handle_t)handle);

    return IPC_OK;
}

//-----------------------------------------------------------------------------

#endif //__IPC_LINUX__

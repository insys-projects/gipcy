
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
#include <errno.h>
#include <error.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>
#include <limits.h>

//-----------------------------------------------------------------------------

#ifndef SHM_R
#define SHM_R 0x100
#endif

#ifndef SHM_W
#define SHM_W 0x80
#endif

#define SVSHM_MODE ( SHM_R | SHM_W | SHM_R>>3 | SHM_R>>6 )

//! Определяет флаги доступа к разделяемой памяти SYSTEM V
#define IPC_SVSHM_MODE   (SHM_R | SHM_W | SHM_R >> 3 | SHM_R >> 6)

//-----------------------------------------------------------------------------

IPC_handle IPC_createSharedMemory(const IPC_str *name, int size)
{
    DEBUG_PRINT("%s(%s)\n", __FUNCTION__, name );

    ipc_handle_t h = allocate_ipc_object(name, IPC_typeSharedMem);
    if(!h) return NULL;

    h->ipc_size = size;

    h->ipc_descr.ipc_shm = shmget(h->ipc_key, size, IPC_SVSHM_MODE | IPC_CREAT | IPC_EXCL);
    if(h->ipc_descr.ipc_shm < 0) {
        if(errno == EEXIST) {

            h->ipc_descr.ipc_shm = shmget(h->ipc_key, 0, IPC_SVSHM_MODE);
            if(h->ipc_descr.ipc_shm < 0) {
                DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
                return NULL;
            } else {

                DEBUG_PRINT("%s(): open shared memory - %s\n", __FUNCTION__, name );
                return h;
            }

        } else {

            DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
            return NULL;

        }
    }

    DEBUG_PRINT("%s(): create shared memory - %s\n", __FUNCTION__, name );

    return h;
}

//-----------------------------------------------------------------------------

IPC_handle IPC_createSharedMemoryEx(const IPC_str *name, int size, int *alreadyCreated)
{
    DEBUG_PRINT("%s(%s)\n", __FUNCTION__, name );

    ipc_handle_t h = allocate_ipc_object(name, IPC_typeSharedMem);
    if(!h) return NULL;

    h->ipc_size = size;

    h->ipc_descr.ipc_shm = shmget(h->ipc_key,size, IPC_SVSHM_MODE | IPC_CREAT | IPC_EXCL);
    if(h->ipc_descr.ipc_shm < 0) {

        if(errno == EEXIST) {

            h->ipc_descr.ipc_shm = shmget(h->ipc_key, 0, IPC_SVSHM_MODE);
            if(h->ipc_descr.ipc_shm < 0) {
                DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
                return NULL;
            } else {
                if(alreadyCreated) *alreadyCreated = 1;
                DEBUG_PRINT("%s(): open shared memory - %s\n", __FUNCTION__, name );
                return h;
            }

        } else {

            DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno));
            return NULL;
        }
    }

    if(alreadyCreated) *alreadyCreated = 0;

    DEBUG_PRINT("%s(): create shared memory - %s\n", __FUNCTION__, name);

    return h;
}

//-----------------------------------------------------------------------------

IPC_handle IPC_openSharedMemory(const IPC_str *name)
{
    DEBUG_PRINT("%s(%s)\n", __FUNCTION__, name );

    ipc_handle_t h = allocate_ipc_object(name, IPC_typeSharedMem);
    if(!h) return NULL;

    h->ipc_size = 0;

    h->ipc_descr.ipc_shm = shmget(h->ipc_key,0, IPC_SVSHM_MODE);
    if(h->ipc_descr.ipc_shm < 0) {

        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
        return NULL;
    }

    DEBUG_PRINT("%s(): create shared memory - %s\n", __FUNCTION__, name);

    return h;
}

//-----------------------------------------------------------------------------

void* IPC_mapSharedMemory(const  IPC_handle handle)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return NULL;

    DEBUG_PRINT("%s(): %s\n", __FUNCTION__, h->ipc_name);

    void *mem = shmat(h->ipc_descr.ipc_shm, NULL, 0);
    if(mem == MAP_FAILED) {
        return NULL;
    }

    h->ipc_data = mem;

    return mem;
}

//-----------------------------------------------------------------------------

int IPC_unmapSharedMemory(const  IPC_handle handle)
{
    ipc_handle_t h = (ipc_handle_t)handle;

    if(!h) return -EINVAL;
    if(!h->ipc_data) return -EINVAL;

    DEBUG_PRINT("%s(): %s\n", __FUNCTION__, h->ipc_name);

    int res = shmdt(h->ipc_data);
    if(res < 0) {
        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno));
        return res;
    }

    h->ipc_data = NULL;

    return 0;
}

//-----------------------------------------------------------------------------

int IPC_deleteSharedMemory(IPC_handle handle)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return -1;

    DEBUG_PRINT("%s(): %s\n", __FUNCTION__, h->ipc_name);

    if(h->ipc_data) {
       IPC_unmapSharedMemory(handle);
    }

    struct shmid_ds id;

    if(shmctl(h->ipc_descr.ipc_shm,IPC_STAT,&id) < 0) {
        int err = errno;
        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
        return err;
    }

    if(id.shm_nattch == 0) {

        if(shmctl(h->ipc_descr.ipc_shm,IPC_RMID,NULL) < 0) {

            int err = errno;
            DEBUG_PRINT("%s(): error delete shared memory - %s. %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
            return err;

        }

        DEBUG_PRINT("%s(): shared memory deleted - %s\n", __FUNCTION__, h->ipc_name );

        delete_ipc_object((ipc_handle_t)h);

    } else {

        DEBUG_PRINT("%s(): shared memory is using. nattach = %d\n", __FUNCTION__, (int)id.shm_nattch );
    }

    return IPC_OK;
}

//-----------------------------------------------------------------------------

#endif //__IPC_LINUX__

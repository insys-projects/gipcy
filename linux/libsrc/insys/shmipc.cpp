
#ifdef __IPC_LINUX__

//#define __VERBOSE__

#include "linipc.h"
#include "ipcioctl.h"

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

static void* shared_memory_open(const IPC_str *name, int size)
{
    DEBUG_PRINT("%s( %s, 0x%x )\n", __FUNCTION__, name, size);

    int fd = ipc_driver_handle();
    if(fd < 0) {
        DEBUG_PRINT("%s(): IPC driver was not opened\n", __FUNCTION__);
        return NULL;
    }

    struct ipc_create_t ipc_create_param;
    memset(&ipc_create_param,0,sizeof(ipc_create_param));

    ipc_create_param.handle = NULL;
    ipc_create_param.value = size;
    snprintf(ipc_create_param.name, sizeof(ipc_create_param.name), "%s", name);

    int res = ioctl(fd,IOCTL_IPC_SHM_OPEN,&ipc_create_param);
    if(res < 0) {
        DEBUG_PRINT("%s(): Error register shared memory", __FUNCTION__ );
        return NULL;
    }

    return ipc_create_param.handle;
}

//-----------------------------------------------------------------------------

static int shared_memory_close(void* handle)
{
    DEBUG_PRINT("%s()\n", __FUNCTION__);

    struct ipc_close_t ipc_close_param;

    int fd = ipc_driver_handle();
    if(fd < 0) {
        DEBUG_PRINT("%s(): IPC driver was not opened\n", __FUNCTION__);
        return -EINVAL;
    }

    ipc_close_param.handle = handle;

    int res = ioctl(fd,IOCTL_IPC_SHM_CLOSE,&ipc_close_param);
    if(res < 0) {
        if(res != -EBUSY) {
            DEBUG_PRINT("%s(): Error unregister shared memory name\n", __FUNCTION__ );
        }
    }

    return res;
}

//-----------------------------------------------------------------------------

IPC_handle IPC_createSharedMemory(const IPC_str *name, int size)
{
    DEBUG_PRINT("%s( %s, [0x%x])\n", __FUNCTION__, name, size);

    int mode = S_IRUSR|S_IWUSR | S_IRGRP|S_IWGRP | S_IROTH|S_IWOTH;
    int oflags = O_RDWR | O_CREAT | O_EXCL;
    ipc_handle_t h = NULL;
    int res = -1;
    int flags = 0;

    void *ipc_drv_handle = shared_memory_open(name, size);
    if(!ipc_drv_handle) {
        goto do_exit;
    }

    h = allocate_ipc_object(name, IPC_typeSharedMem);
    if(!h) {
        goto do_unregister_shm;
        return NULL;
    }

    h->ipc_user = ipc_drv_handle;
    h->ipc_descr.ipc_shm = shm_open(h->ipc_name, oflags, mode);
    if(h->ipc_descr.ipc_shm < 0) {

        if(errno != EEXIST) {
            DEBUG_PRINT("%s(): shm_open(%s) error - %s\n", __FUNCTION__, name, strerror(errno) );
            goto do_free_ipc_object;
        }

        h->ipc_descr.ipc_shm = shm_open(h->ipc_name, O_RDWR, mode);
        if(h->ipc_descr.ipc_shm < 0) {
            DEBUG_PRINT("%s(): shm_open(%s) error - %s\n", __FUNCTION__, name, strerror(errno) );
            goto do_free_ipc_object;
        }

        struct stat st_buf;
        res = fstat(h->ipc_descr.ipc_shm, &st_buf);
        if(res < 0) {
            DEBUG_PRINT("%s(): fstat(%s) error - %s\n", __FUNCTION__, name, strerror(errno) );
            goto do_free_ipc_object;
        }

        if(st_buf.st_size > 0) {
            DEBUG_PRINT("%s(): shm size %s is %ld\n", __FUNCTION__, name, st_buf.st_size );
            goto do_return_ipc_object;
        }

        res = ftruncate(h->ipc_descr.ipc_shm, size);
        if(res < 0) {
            DEBUG_PRINT("%s(): 1 ftruncate(%s) error - %s\n", __FUNCTION__, name, strerror(errno) );
            goto do_free_ipc_object;
        }

        DEBUG_PRINT("%s( %s, 0x%x) - OPEN & INIT\n", __FUNCTION__, name, size);

        goto do_return_ipc_object;
    }

    res = ftruncate(h->ipc_descr.ipc_shm, size);

    if(res < 0) {
        DEBUG_PRINT("%s(): 2 ftruncate(%s) error - %s\n", __FUNCTION__, name, strerror(errno) );
        goto do_free_ipc_object;
    }

    flags = fcntl(h->ipc_descr.ipc_shm, F_GETFD);
    if (flags == -1) {
        DEBUG_PRINT("%s(): fcntl(%s) error - %s\n", __FUNCTION__, name, strerror(errno) );
    } else {
        DEBUG_PRINT("%s(): fcntl(%s) - flags 0x%x\n", __FUNCTION__, name, flags );
        flags &= ~FD_CLOEXEC;
        DEBUG_PRINT("%s(): fcntl(%s) - flags 0x%x\n", __FUNCTION__, name, flags );
    }
    if (fcntl(h->ipc_descr.ipc_shm, F_SETFD, flags) == -1) {
        DEBUG_PRINT("%s(): fcntl(%s) error - %s\n", __FUNCTION__, name, strerror(errno) );
    }

    DEBUG_PRINT("%s( %s, 0x%x) - CREATE\n", __FUNCTION__, name, size);

do_return_ipc_object:
    h->ipc_size = size;
    return h;

do_free_ipc_object:
    delete_ipc_object(h);

do_unregister_shm:
    shared_memory_close(ipc_drv_handle);

do_exit:
    return NULL;
}

//-----------------------------------------------------------------------------

IPC_handle IPC_createSharedMemoryEx(const IPC_str *name, int size, int *alreadyCreated)
{
    int mode = S_IRUSR|S_IWUSR | S_IRGRP|S_IWGRP | S_IROTH|S_IWOTH;
    int oflags = O_RDWR | O_CREAT | O_EXCL;
    ipc_handle_t h = NULL;
    int res = -1;

    void *ipc_drv_handle = shared_memory_open(name, size);
    if(!ipc_drv_handle) {
        goto do_exit;
    }

    h = allocate_ipc_object(name, IPC_typeSharedMem);
    if(!h) {
        goto do_unregister_shm;
    }

    h->ipc_user = ipc_drv_handle;
    h->ipc_descr.ipc_shm = shm_open(h->ipc_name, oflags, mode );
    if(h->ipc_descr.ipc_shm < 0) {

        if(errno != EEXIST)
            goto do_free_ipc_object;

        h->ipc_descr.ipc_shm = shm_open(h->ipc_name, O_RDWR, mode);
        if(h->ipc_descr.ipc_shm < 0)
            goto do_free_ipc_object;

        struct stat st_buf;
        res = fstat(h->ipc_descr.ipc_shm, &st_buf);
        if(res < 0)
            goto do_free_ipc_object;

        if(st_buf.st_size > 0) {
            if(alreadyCreated) *alreadyCreated = 1;
            DEBUG_PRINT("%s( %s, 0x%x) - OPEN\n", __FUNCTION__, name, size);
            goto do_return_ipc_object;
        }

        res = ftruncate(h->ipc_descr.ipc_shm, size);
        if(res < 0)
            goto do_free_ipc_object;

        if(alreadyCreated) *alreadyCreated = 1;

        DEBUG_PRINT("%s( %s, 0x%x) - OPEN & INIT\n", __FUNCTION__, name, size);

        goto do_return_ipc_object;
    }

    if(alreadyCreated) *alreadyCreated = 0;

    res = ftruncate(h->ipc_descr.ipc_shm, size);
    if(res < 0)
        goto do_free_ipc_object;

    DEBUG_PRINT("%s( %s, 0x%x) - CREATE\n", __FUNCTION__, name, size);

do_return_ipc_object:
    h->ipc_size = size;

    return h;

do_free_ipc_object:
    delete_ipc_object(h);

do_unregister_shm:
    shared_memory_close(ipc_drv_handle);

do_exit:
    return NULL;
}

//-----------------------------------------------------------------------------

IPC_handle IPC_openSharedMemory(const IPC_str *name)
{
    DEBUG_PRINT("%s( %s )\n", __FUNCTION__, name);

    int mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    int oflags = O_RDWR;
    ipc_handle_t h = NULL;
    struct stat st_buf;
    int res = -1;

    void *ipc_drv_handle = shared_memory_open(name, 0);
    if(!ipc_drv_handle) {
        goto do_exit;
    }

    h = allocate_ipc_object(name, IPC_typeSharedMem);
    if(!h) {
        goto do_unregister_shm;
    }

    h->ipc_user = ipc_drv_handle;
    h->ipc_descr.ipc_shm = shm_open(h->ipc_name, oflags, mode);
    if(h->ipc_descr.ipc_shm < 0) {
        goto do_free_ipc_object;
    }

    //--------------------------
    res = fstat(h->ipc_descr.ipc_shm, &st_buf);
    if(res < 0) {
        goto do_free_ipc_object;
    }

    if(st_buf.st_size > 0)
        h->ipc_size = st_buf.st_size;
    //--------------------------

    DEBUG_PRINT("%s(): open shared memory - %s\n", __FUNCTION__, name);

    return h;

do_free_ipc_object:
    delete_ipc_object(h);

do_unregister_shm:
    shared_memory_close(ipc_drv_handle);

do_exit:
    return NULL;
}


//-----------------------------------------------------------------------------

void* IPC_mapSharedMemory(const  IPC_handle handle)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return NULL;

    DEBUG_PRINT("%s(): map shared memory %s\n", __FUNCTION__, h->ipc_name);

    if(h->ipc_size == 0) {
        DEBUG_PRINT("%s(): map shared memory failed %s. Size = %d bytes\n", __FUNCTION__, h->ipc_name, h->ipc_size);
        return NULL;
    }

    void *mem = mmap(NULL, h->ipc_size, PROT_READ|PROT_WRITE, MAP_SHARED, h->ipc_descr.ipc_shm, 0);

    close(h->ipc_descr.ipc_shm);

    if(mem == MAP_FAILED) {
        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno));
        return NULL;
    }

    h->ipc_data = mem;

    DEBUG_PRINT("%s(%s): mapped address - %p [0x%x]\n", __FUNCTION__, h->ipc_name, mem, h->ipc_size);

    // On Zynq (kernel 4.9) we can have only 16 shared memory objects !!!!
/*
    uint32_t *pmem = (uint32_t*)mem;
    uint32_t dummy_read = pmem[0];
    dummy_read = dummy_read;
    DEBUG_PRINT("%s(%s): Dummy read address - %p [0x%x] - DONE\n", __FUNCTION__, h->ipc_name, mem, h->ipc_size);
    pmem[0] = dummy_read;
    DEBUG_PRINT("%s(%s): Dummy write address - %p [0x%x] - DONE\n", __FUNCTION__, h->ipc_name, mem, h->ipc_size);
*/

    return mem;
}

//-----------------------------------------------------------------------------

int IPC_unmapSharedMemory(const  IPC_handle handle)
{
    ipc_handle_t h = (ipc_handle_t)handle;

    if(!h) return IPC_INVALID_HANDLE;
    if(!h->ipc_data) return IPC_GENERAL_ERROR;

    DEBUG_PRINT("%s(): %s\n", __FUNCTION__, h->ipc_name);

    int res = munmap(h->ipc_data, h->ipc_size);
    if(res < 0) {
        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno));
        return IPC_GENERAL_ERROR;
    }

    h->ipc_data = NULL;
    h->ipc_size = 0;

    return IPC_OK;
}

//-----------------------------------------------------------------------------

int IPC_deleteSharedMemory(IPC_handle handle)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return -1;

    if(h->ipc_type != IPC_typeSharedMem)
        return IPC_INVALID_HANDLE;

    DEBUG_PRINT("%s(): %s\n", __FUNCTION__, h->ipc_name);

    if(h->ipc_data) {
        IPC_unmapSharedMemory(handle);
    }

    int res = shared_memory_close(h->ipc_user);
    if(res < 0) {
        if(res == -EBUSY) {
            DEBUG_PRINT("%s(): shared_memory_close(%s) error - %s\n", __FUNCTION__, h->ipc_name, strerror(errno) );
            return IPC_OK;
        } else {
            DEBUG_PRINT("%s(): shared_memory_close(%s) error - %s\n", __FUNCTION__, h->ipc_name, strerror(errno) );
            DEBUG_PRINT("%s(): Error unregister shared memory name in IPC driver\n", __FUNCTION__ );
            return IPC_GENERAL_ERROR;
        }
    }

    res = shm_unlink(h->ipc_name);
    if(res < 0) {
        DEBUG_PRINT("%s(): shm_unlink(%s) error - %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
        return IPC_GENERAL_ERROR;
    }
    DEBUG_PRINT("%s(): DELETE OK\n", __FUNCTION__);

    return IPC_OK;
}

//-----------------------------------------------------------------------------

#endif //__IPC_LINUX__

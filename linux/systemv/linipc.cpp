
#ifdef __IPC_LINUX__

#ifndef __LINIPC_H__
#include "linipc.h"
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
#include <pthread.h>

//-----------------------------------------------------------------------------

static char* create_ipc_name(const char *name)
{
    size_t prefix_len = strlen("/tmp/ipclib/");
    size_t name_len = name ? strlen(name) : strlen("ipc_XXXXXXXX");
    size_t len = prefix_len + name_len + 2;

    char *ipc_name = (char*)malloc(len);
    if(!ipc_name) {
        DEBUG_PRINT("%s(): %s", __FUNCTION__, strerror(errno));
        return NULL;
    }

    memset(ipc_name, 0, len);

    if(name) {
        snprintf(ipc_name, len, "%s%s", "/tmp/ipclib/", name);
    } else {
        char tmp_name[] = {"/tmp/ipclib/ipc_XXXXXX"};
        char *local_name = mktemp(tmp_name);
        if(!local_name) {
            free(ipc_name);
            DEBUG_PRINT("%s(): Cant create temporary file name: %s", __FUNCTION__, tmp_name);
            return NULL;
        }
        snprintf(ipc_name, len, "%s", local_name);
    }

    return ipc_name;
}

//-----------------------------------------------------------------------------

static char* create_raw_name(const char *name)
{
    size_t len = strlen(name)+1;

    if(!name)
        return NULL;

    char *ipc_name = (char*)malloc(len);
    if(!ipc_name) {
        DEBUG_PRINT("%s(): %s", __FUNCTION__, strerror(errno));
        return NULL;
    }

    memset(ipc_name, 0, len);

    snprintf(ipc_name, len, "%s", name);

    return ipc_name;
}

//-----------------------------------------------------------------------------

static void delete_ipc_name(char *ipc_name)
{
    free(ipc_name);
}

//-----------------------------------------------------------------------------

static int create_ipc_file(const char *full_name)
{
    if(!full_name) {
        return -EINVAL;
    }

    int fd = open(full_name,
                  O_EXCL | O_CREAT | O_RDWR,
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if(fd < 0) {
        if(errno != EEXIST) {
            int err = errno;
            DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(err) );
            return err;
        } else {
            DEBUG_PRINT("%s(): Open IPC file %s\n", __FUNCTION__, full_name );
            return 0;
        }
    } else {
        close(fd);
    }

    DEBUG_PRINT("%s(): Create IPC file %s\n", __FUNCTION__, full_name );

    return 0;
}

//-----------------------------------------------------------------------------

static int delete_ipc_file(const char *full_name)
{
    if(!full_name)
        return -EINVAL;

    if(unlink(full_name) < 0) {
        if(errno != ENOENT) {
            int err = errno;
            DEBUG_PRINT("%s\n", strerror(err) );
            return err;
        }
    } else {
        DEBUG_PRINT("%s(): Delete IPC file %s\n", __FUNCTION__, full_name );
    }

    return 0;
}

//-----------------------------------------------------------------------------

static key_t get_ipc_key( ipc_handle_t h )
{
    key_t key = ftok(h->ipc_name, 0);
    if(key == -1) {
        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno) );
        return (key_t)-1;
    }

    return key;
}

//-----------------------------------------------------------------------------

ipc_handle_t allocate_ipc_object(const char *name, IPC_type type)
{
    mkdir("/tmp/ipclib", 0777);

    ipc_handle_t h = (ipc_handle_t)malloc(sizeof(struct ipc_t));
    if(!h) {
        return NULL;
    }

    memset(h,0,sizeof(struct ipc_t));

    h->ipc_type = type;

    if((type == IPC_typeThread) ||
       (type == IPC_typeLibrary) ||
       (type == IPC_typeFile) ||
       (type == IPC_typeDevice))
    {
        h->ipc_name = create_raw_name(name);
        return h;

    } else {

        h->ipc_name = create_ipc_name(name);
    }

    if(!h->ipc_name) {
        free(h);
        return NULL;
    }

    if(create_ipc_file(h->ipc_name) < 0) {
        delete_ipc_name(h->ipc_name);
        free(h);
        return NULL;
    }

    h->ipc_key = get_ipc_key(h);
    if(h->ipc_key == (key_t)-1) {
        delete_ipc_file(h->ipc_name);
        delete_ipc_name(h->ipc_name);
        free(h);
        return NULL;
    }

    return h;
}

//-----------------------------------------------------------------------------

void delete_ipc_object(ipc_handle_t h)
{
    if(!h) return;

    //! сделать что-нить зависящее от типа IPC
    switch(h->ipc_type) {
    case IPC_typeSemaphore:
    case IPC_typeSharedMem:
    case IPC_typeMutex:
    case IPC_typeEvent:
        {
            if(h->ipc_name)
                delete_ipc_file(h->ipc_name);
        } break;
    case IPC_typeThread:
    case IPC_typeLibrary:
    case IPC_typeFile:
    case IPC_typeDevice:
    case IPC_typeDir:
        break;
    }

    if(h->ipc_name)
        delete_ipc_name(h->ipc_name);

    free(h);
}

//-----------------------------------------------------------------------------

bool is_ok_remove(ipc_handle_t h)
{
    if(!h)
        return false;

    if((h->ipc_type != IPC_typeSemaphore) &&
       (h->ipc_type != IPC_typeMutex) &&
       (h->ipc_type != IPC_typeEvent))
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
        DEBUG_PRINT( "%s(): - %s [free]\n", __FUNCTION__, fname );
        return true;
    }

    DEBUG_PRINT( "%s(): - %s [locked]\n", __FUNCTION__, fname );

    return false;
}

//-----------------------------------------------------------------------------

int IPC_init(void)
{
    return 0;
}

//-----------------------------------------------------------------------------

int IPC_cleanup(void)
{
    return 0;
}

//-----------------------------------------------------------------------------

#endif //__IPC_LINUX__

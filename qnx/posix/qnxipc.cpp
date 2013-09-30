
#ifdef __IPC_QNX__

#ifndef __QNXIPC_H__
#include "qnxipc.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>

//-----------------------------------------------------------------------------
#define IPC_SHM_PREFIX "/"
//-----------------------------------------------------------------------------

static char* create_raw_name(const char *name)
{
    size_t len = strlen(name)+1;

    if(!name)
        return NULL;

    char *ipc_name = (char*)malloc(len);
    if(!ipc_name) {
        fprintf(stderr, "%s(): %s\n", __FUNCTION__, strerror(errno));
        return NULL;
    }

    memset(ipc_name, 0, len);

    snprintf(ipc_name, len, "%s", name);

    //fprintf(stderr, "%s(): %s\n", __FUNCTION__, ipc_name);

    return ipc_name;
}

//-----------------------------------------------------------------------------

static void delete_ipc_name(char *ipc_name)
{
    free(ipc_name);
}

//-----------------------------------------------------------------------------

ipc_handle_t allocate_ipc_object(const char *name, IPC_type type)
{
    ipc_handle_t h = (ipc_handle_t)malloc(sizeof(struct ipc_t));
    if(!h) {
        return NULL;
    }

    memset(h,0,sizeof(struct ipc_t));

    h->ipc_type = type;
    if(h->ipc_type == IPC_typeSharedMem) {

        char tmpName[PATH_MAX] = {'\0'};

        snprintf(tmpName, PATH_MAX, "%s%s", IPC_SHM_PREFIX, name);

        int len = strlen(tmpName)+1;

        h->ipc_name = (char*)malloc(len);
        if(!h->ipc_name) {
            fprintf(stderr, "%s(): %s\n", __FUNCTION__, strerror(errno));
            return NULL;
        }

        snprintf(h->ipc_name, len, "%s", tmpName);

        //fprintf(stderr, "%s(): %s\n", __FUNCTION__, h->ipc_name);

    } else {
        h->ipc_name = create_raw_name(name);
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
    case IPC_typeThread:
    case IPC_typeLibrary:
    case IPC_typeFile:
    case IPC_typeDevice:
    case IPC_typeDir:
    case IPC_typeSocket:
        break;
    }

    if(h->ipc_name)
        delete_ipc_name(h->ipc_name);

    free(h);
}

//-----------------------------------------------------------------------------

bool chechk_handle(ipc_handle_t h, int h_type)
{
    if(!h) return false;

    if(h->ipc_type != h_type)
        return false;

    return true;
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

#endif //__IPC_QNX__

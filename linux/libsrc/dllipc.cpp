
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

IPC_handle IPC_openLibraryEx(const IPC_str *baseName, unsigned param)
{
	return IPC_openLibrary(baseName, param);
}

//-----------------------------------------------------------------------------

IPC_handle IPC_openLibrary(const IPC_str *name, unsigned param)
{
    ipc_handle_t h = allocate_ipc_object(name, IPC_typeLibrary);
    if(!h) return NULL;

    h->ipc_descr.ipc_lib = dlopen(name, RTLD_LAZY);
    if(!h->ipc_descr.ipc_lib) {
        //fprintf( stderr, "%s(): Error - %s\n", __FUNCTION__, dlerror());
        DEBUG_PRINT( "%s(): Error load library %s. %s\n", __FUNCTION__, name, dlerror());
        delete_ipc_object(h);
        return NULL;
    }

    return h;
}

//-----------------------------------------------------------------------------

void* IPC_getEntry(IPC_handle handle, const char *entryName)
{
    //DEBUG_PRINT("%s()\n", __FUNCTION__);

    if(!handle || !entryName)
        return NULL;

    ipc_handle_t h = (ipc_handle_t)handle;

    return dlsym(h->ipc_descr.ipc_lib, entryName);;
}

//-----------------------------------------------------------------------------

int IPC_closeLibrary(IPC_handle handle)
{
    DEBUG_PRINT("%s()\n", __FUNCTION__);

    if(!handle)
        return IPC_INVALID_HANDLE;

    ipc_handle_t h = (ipc_handle_t)handle;

    int res = dlclose(h->ipc_descr.ipc_lib);
    if(res < 0) {
        DEBUG_PRINT( "%s(): Error unload library. %s\n", __FUNCTION__, dlerror());
        return IPC_GENERAL_ERROR;
    }

    delete_ipc_object(h);

    return IPC_OK;
}

//-----------------------------------------------------------------------------

char* IPC_libraryError(void)
{
    return dlerror();
}

//-----------------------------------------------------------------------------

#endif //__IPC_LINUX__


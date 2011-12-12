
#ifndef __DLLIPC_H__
#define __DLLIPC_H__

#ifndef __IPCLIB_H__
    #include "ipclib.h"
#endif

//----------------------------------------------------------------------
// Функции для работы с разделяемыми библиотеками
//----------------------------------------------------------------------

extern "C" {

IPC_handle IPC_openLibrary(const IPC_str *name, unsigned param);
void* IPC_getEntry(IPC_handle handle, const char *entryName);
int IPC_closeLibrary(IPC_handle handle);

};

#endif //__DLLIPC_H__


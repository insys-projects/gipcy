#ifndef __IPCTYPE_H__
#define __IPCTYPE_H__

enum IPC_type {

    IPC_typeSemaphore,
    IPC_typeMutex,
    IPC_typeEvent,
    IPC_typeThread,
    IPC_typeSharedMem,
    IPC_typeLibrary,
    IPC_typeFile,
    IPC_typeDevice
};

#endif // __IPCTYPE_H__

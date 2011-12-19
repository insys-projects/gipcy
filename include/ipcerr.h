#ifndef __IPCERR_H__
#define __IPCERR_H__

//----------------------------------------------------------------------

enum IPC_error {

    IPC_ok,
    IPC_invalidHandle,
    IPC_generalError,
    IPC_noMemory,
    IPC_timeout,
    IPC_interrupted,
    IPC_overflow,
    IPC_abandoned
};

//----------------------------------------------------------------------

void IPC_errorString(IPC_error err);

//----------------------------------------------------------------------

#endif // __IPCERR_H__

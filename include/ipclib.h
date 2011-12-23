#ifndef __IPCLIB_H__
#define __IPCLIB_H__

//----------------------------------------------------------------------

#ifndef __IPCERR_H__
    #include "ipcerr.h"
#endif
#ifndef __IPCTYPE_H__
    #include "ipctype.h"
#endif
#ifndef __STRIPC_H__
    #include "stripc.h"
#endif

//----------------------------------------------------------------------

#include <stdarg.h>

//----------------------------------------------------------------------

#ifdef __IPC_LINUX__
#include <pthread.h>
#include <errno.h>
#endif

#ifdef __IPC_WIN__
#include <windows.h>
#include <winioctl.h>
//#include <dos.h>
#include <conio.h>
#include <crtdbg.h>
#endif

//----------------------------------------------------------------------

//! IPC_handle - внешний тип данных используемый для работы с объектами IPC
typedef void* IPC_handle;

//----------------------------------------------------------------------

#endif // __IPCLIB_H__

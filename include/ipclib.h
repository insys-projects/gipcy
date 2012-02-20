#ifndef __IPCLIB_H__
#define __IPCLIB_H__

//----------------------------------------------------------------------

#ifndef __IPCERR_H__
    #include "ipcerr.h"
#endif
//#ifndef __IPCTYPE_H__
//    #include "ipctype.h"
//#endif
//#ifndef __STRIPC_H__
//    #include "stripc.h"
//#endif

enum IPC_type {

    IPC_typeSemaphore,
    IPC_typeMutex,
    IPC_typeEvent,
    IPC_typeThread,
    IPC_typeSharedMem,
    IPC_typeLibrary,
    IPC_typeFile,
    IPC_typeDevice,
    IPC_typeDir
};

//----------------------------------------------------------------------
// Функции и типы для работы со строками
//----------------------------------------------------------------------

//! Определим тип указателя на строку,
//! чтобы исключить директивы препроцессора
//! в интерфейсных функциях

#ifdef __linux__
    typedef char           IPC_str;
    //#define DIR_DELIMITER "/"
#else
    #ifdef _WIN64
        typedef wchar_t    IPC_str;
        //#define DIR_DELIMITER L"\\"
    #else
        typedef char       IPC_str;
        //#define DIR_DELIMITER "\\"
    #endif
#endif

//----------------------------------------------------------------------

#include <stdarg.h>

//----------------------------------------------------------------------

#ifdef __IPC_LINUX__
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/mman.h>
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

#ifdef GIPCY_EXPORTS
 #define GIPCY_API __declspec(dllexport)
#else
 #define GIPCY_API
// #define GIPCY_API __declspec(dllimport)
#endif // GIPCY_EXPORTS

#endif // __IPCLIB_H__

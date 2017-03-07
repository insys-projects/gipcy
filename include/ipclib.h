#ifndef __IPCLIB_H__
#define __IPCLIB_H__

//----------------------------------------------------------------------

#ifndef __IPCERR_H__
    #include "ipcerr.h"
#endif

#ifndef __IPCTYPE_H__
    #include "ipctype.h"
#endif

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
    #ifdef _UNICODE
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
#include <sys/ioctl.h>
#endif

#ifdef __IPC_WIN__
#include <windows.h>
#include <winioctl.h>
//#include <dos.h>
#include <conio.h>
#include <crtdbg.h>
#endif

//----------------------------------------------------------------------

//! IPC_handle - внешний тип данных, используемый для работы с объектами IPC
typedef void* IPC_handle;

//----------------------------------------------------------------------
// IPC_TIMEVAL - внешний тип данных, используемый для работы с функциями измерения времени
#ifdef __IPC_LINUX__
typedef struct timeval IPC_TIMEVAL;
#endif

#ifdef __IPC_WIN__
typedef LARGE_INTEGER IPC_TIMEVAL;
#endif

//----------------------------------------------------------------------

#ifdef GIPCY_EXPORTS
 #define GIPCY_API __declspec(dllexport)
#else
 #define GIPCY_API
// #define GIPCY_API __declspec(dllimport)
#endif // GIPCY_EXPORTS

//----------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

//! Функция инициализации библиотеки IPC
/*!
  Выполняет инициализацию библиотеки IPC. Необходимо вызвать один раз
  в теле программы. Выполняет открытия драйвера IPC. Необходима только в linux.
*/
int IPC_init(void);

//! Функция деинициализации библиотеки IPC
/*!
  Выполняет деинициализацию библиотеки IPC. Необходимо вызвать один раз
  в теле программы, после работы с библиотекой. Выполняет закрытие драйвера IPC.
  Необходима только в linux.
*/
int IPC_cleanup(void);

//! Функция возвращает дескриптор устройства IPC
GIPCY_API	size_t	IPC_getDescriptor(IPC_handle handle);

#ifdef __cplusplus
};
#endif

//----------------------------------------------------------------------

#endif // __IPCLIB_H__

#ifndef __THREADIPC_H__
#define __THREADIPC_H__

#ifndef __IPCLIB_H__
    #include "ipclib.h"
#endif
#ifndef __SEMIPC_H__
    #include "semipc.h"
#endif

//----------------------------------------------------------------------
// Функции для работы с потоками
//----------------------------------------------------------------------

#define IPC_CREATE_THREAD_STARTED   0x0
#define IPC_CREATE_THREAD_PAUSED    0x1
#define IPC_THREAD_START            0x2
#define IPC_THREAD_STOP             0x4
#define IPC_THREAD_RESUME           0x8
#define IPC_THREAD_EXIT             0x10

#if defined(__linux__) || defined(__QNX__)

#include <pthread.h>
#define __IPC_API
typedef void* thread_value;                             //!< Тип значения, возвращаемого функцией потока при завершении

#else

#define __IPC_API __stdcall
typedef unsigned int thread_value;                      //!< Тип значения, возвращаемого функцией потока при завершении

#endif

typedef thread_value (__IPC_API thread_func) (void*);    //!< Тип указателя на функцию потока (зависит от операционной системы)

//! Структура передаваемая функции потока в качестве параметра (зависит от операционной системы)
struct thread_param {

    thread_func*    threadFunction;      //!< Указатель на функцию потока (user input)
    void*           threadData;          //!< Данные передаваемые для функции потока (user input)
    IPC_handle      threadMutex;         //!< Мьютекс на котором поток будет ожидать команд управления (user input)
    thread_value    threadExitValue;     //!< Значение возвращенное функцией потока (output)
};

#ifdef __cplusplus
extern "C" {
#endif

    //! Функция создает поток
    /*!
    \param name - имя потока
    \param function - указатель на рабочую функцию потока
    \param param - параметры передаваемые рабочей функции потока
    \param flags - флаги потока
	\return в случае ошибки возвращает NULL
    */
GIPCY_API    IPC_handle IPC_createThread(const IPC_str *name, thread_func* function, void* param);

    //! Функция создает поток
    /*! Для синхронизации действий с потоком исрользовать семафор
    \param name - имя потока
    \param tp - парметры для потока, заполняются пользователем
    \param flags - флаги потока
	\return в случае ошибки возвращает NULL
    */
GIPCY_API    IPC_handle IPC_createThreadEx(const IPC_str *name, struct thread_param *tp, int flags);

    //! Функция разрешает потоку выполняться (только LINUX)
    /*!
    \param handle - дескриптор потока
    */
GIPCY_API    int IPC_startThread(const IPC_handle handle);

    //! Функция приостанавливает выполнение потока
    /*!
    \param handle - дескриптор потока
    */
GIPCY_API    int IPC_stopThread(const IPC_handle handle);

    //! Функция ожидает завершения потока
    /*!
    \param handle - дескриптор потока
    \param timeout - время ожидания в мс
    */
GIPCY_API    int IPC_waitThread(const IPC_handle handle, int timeout);

    //! Функция останавливает поток и удаляет дескриптор потока
    /*!
    \param handle - дескриптор разделяемой памяти
    \return в случае ошибки возвращает IPC_INVALID_HANDLE или IPC_GENERAL_ERROR
    */
GIPCY_API    int IPC_deleteThread(IPC_handle handle);

    //! Функция возвращает идентификатор потока
    /*!
    \return Возвращает идентификатор потока
    */
GIPCY_API    size_t IPC_threadID(void);

#ifdef _WIN32
    typedef int IPC_tls_key;
#else
    typedef pthread_key_t IPC_tls_key;
#endif
GIPCY_API    IPC_tls_key IPC_createTlsKey(void);
GIPCY_API    void* IPC_tlsGetValue(IPC_tls_key key);
GIPCY_API    int IPC_tlsSetValue(IPC_tls_key key, void *ptr);
GIPCY_API    int IPC_deleteTlsKey(IPC_tls_key key);

#ifdef __cplusplus
}
#endif

#endif //__THREADIPC_H__

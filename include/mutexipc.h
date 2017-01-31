
#ifndef __MUTEXIPC_H__
#define __MUTEXIPC_H__

#ifndef __IPCLIB_H__
    #include "ipclib.h"
#endif

//----------------------------------------------------------------------
// Функции для работы с мютексами
//----------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

    //! Функция создает именованный мютекс
    /*!
    \param name - имя мютекса
    \param value - начальное значение мютекса
	\return в случае ошибки возвращает NULL
    */
GIPCY_API	IPC_handle IPC_createMutex(const IPC_str *name, bool value);

    //! Функция захватывает мютекс или ждет его освобождения
    /*!
    \param handle - дескриптор мютекса
    \param timeout - время ожидания операции
	\return в случае ошибки возвращает IPC_INVALID_HANDLE или IPC_WAIT_TIMEOUT или IPC_WAIT_ABANDONED
    */
GIPCY_API    int IPC_captureMutex(const IPC_handle handle, int timeout);

    //! Функция освобождает занятый мютекс
    /*!
    \param handle - дескриптор мютекса
	\return в случае ошибки возвращает IPC_INVALID_HANDLE или IPC_GENERAL_ERROR
    */
GIPCY_API    int IPC_releaseMutex(const IPC_handle handle);

    //! Функция удаляет именованный объект мютекса
    /*!
    \param handle - дескриптор мютекса
	\return в случае ошибки возвращает IPC_INVALID_HANDLE или IPC_GENERAL_ERROR
    */
GIPCY_API    int IPC_deleteMutex(IPC_handle handle);
#ifdef __cplusplus
}
#endif

#endif //__MUTEXIPC_H__

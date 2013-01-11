
#ifndef __EVENTIPC_H__
#define __EVENTIPC_H__

#ifndef __IPCLIB_H__
    #include "ipclib.h"
#endif

//----------------------------------------------------------------------
// Функции для работы с событиями
//----------------------------------------------------------------------

extern "C" {

    //! Функция создает событие
    /*!
    \param name - имя события
    \param manual - событие с ручным сбросом
    \param value - начальное значение события
	\return в случае ошибки возвращает NULL
    */
GIPCY_API    IPC_handle IPC_createEvent(const IPC_str *name, bool manual, bool value);

    //! Функция ожидает перехода события в сигнальное состояние
    /*!
    \param handle - дескриптор события
    \param timeout - время ожидания операции
	\return в случае ошибки возвращает IPC_INVALID_HANDLE или IPC_WAIT_TIMEOUT или IPC_WAIT_ABANDONED
    */
GIPCY_API    int IPC_waitEvent(const IPC_handle handle, int timeout);

    //! Функция переводит событие в сигнальное состояние
    /*!
    \param handle - дескриптор события
	\return в случае ошибки возвращает IPC_INVALID_HANDLE или IPC_GENERAL_ERROR
    */
GIPCY_API    int IPC_setEvent(const  IPC_handle handle);

    //! Функция переводит событие в несигнальное состояние
    /*!
    \param handle - дескриптор события
	\return в случае ошибки возвращает IPC_INVALID_HANDLE или IPC_GENERAL_ERROR
    */
GIPCY_API    int IPC_resetEvent(const  IPC_handle handle);

    //! Функция удаляет объект события
    /*!
    \param handle - дескриптор события
	\return в случае ошибки возвращает IPC_INVALID_HANDLE или IPC_GENERAL_ERROR
    */
GIPCY_API    int IPC_deleteEvent(IPC_handle handle);
}

#endif //__MUTEXIPC_H__

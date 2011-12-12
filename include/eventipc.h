
#ifndef __EVENTIPC_H__
#define __EVENTIPC_H__

#ifndef __IPCLIB_H__
    #include "ipclib.h"
#endif

//----------------------------------------------------------------------
// Функции для работы с событиями
//----------------------------------------------------------------------

    //! Функция создает событие
    /*!
    \param name - имя события
    \param value - начальное значение события
    */
    extern "C" IPC_handle IPC_createEvent(const IPC_str *name, bool manual, bool value);

    //! Функция ожидает перехода события в сигнальное состояние
    /*!
    \param handle - дескриптор события
    \param timeout - время ожидания операции
    */
    extern "C" int IPC_waitEvent(const IPC_handle handle, int timeout);

    //! Функция переводит событие в сигнальное состояние
    /*!
    \param handle - дескриптор события
    */
    extern "C" int IPC_setEvent(const  IPC_handle handle, int value);

    //! Функция переводит событие в несигнальное состояние
    /*!
    \param handle - дескриптор события
    */
    extern "C" int IPC_resetEvent(const  IPC_handle handle, int value);

    //! Функция удаляет объект события
    /*!
    \param handle - дескриптор события
    */
    extern "C" int IPC_deleteEvent(IPC_handle handle);

#endif //__MUTEXIPC_H__


#ifndef __MUTEXIPC_H__
#define __MUTEXIPC_H__

#ifndef __IPCLIB_H__
    #include "ipclib.h"
#endif

//----------------------------------------------------------------------
// Функции для работы с мютексами
//----------------------------------------------------------------------

extern "C" {

    //! Функция создает именованный мютекс
    /*!
    \param name - имя мютекса
    \param value - начальное значение мютекса
    */
    IPC_handle IPC_createMutex(const IPC_str *name, bool value);

    //! Функция захватывает мютекс или ждет его освобождения
    /*!
    \param handle - дескриптор мютекса
    \param timeout - время ожидания операции
    */
    int IPC_captureMutex(const IPC_handle handle, int timeout);

    //! Функция освобождает занятый мютекс
    /*!
    \param handle - дескриптор мютекса
    */
    int IPC_releaseMutex(const IPC_handle handle);

    //! Функция удаляет именованный объект мютекса
    /*!
    \param handle - дескриптор мютекса
    */
    int IPC_deleteMutex(IPC_handle handle);
}

#endif //__MUTEXIPC_H__

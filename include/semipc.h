
#ifndef __SEMIPC_H__
#define __SEMIPC_H__

#ifndef __IPCLIB_H__
    #include "ipclib.h"
#endif

//----------------------------------------------------------------------
// Функции для работы с семаформаи
//----------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

    //! Функция создает именованный семафор
    /*!
    \param name - имя семафора
    \param value - начальное значение семафора
    */
GIPCY_API    IPC_handle IPC_createSemaphore(const IPC_str *name, int value);

    //! Функция создает именованный семафор с дополнительным параметром
    /*!
    \param name - имя семафора
    \param value - начальное значение семафора
    */
GIPCY_API    IPC_handle IPC_createSemaphoreEx(const IPC_str *name, int value, ... );

    //! Функция захватывает семафор или ждет его освобождения
    /*!
    \param handle - дескриптор семафора
    \param timeout - время ожидания операции
    */
GIPCY_API    int IPC_lockSemaphore(const IPC_handle handle, int timeout);

    //! Функция освобождает занятый семафор
    /*!
    \param handle - дескриптор семафора
    */
GIPCY_API    int IPC_unlockSemaphore(const IPC_handle handle);

    //! Функция удаляет именованный объект семафора
    /*!
    \param handle - дескриптор семафора
    */
GIPCY_API    int IPC_deleteSemaphore(IPC_handle handle);

#ifdef __cplusplus
}
#endif

#endif //__SEMIPC_H__

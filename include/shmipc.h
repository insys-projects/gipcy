
#ifndef __SHMIPC_H__
#define __SHMIPC_H__

#ifndef __IPCLIB_H__
    #include "ipclib.h"
#endif

//----------------------------------------------------------------------
// Функции для работы с разделяемой памятью
//----------------------------------------------------------------------

extern "C" {

    //! Функция создает именованный объект разделяемой памяти
    /*!
    \param name - имя объекта разделяемой памяти
    \param size - размер разделяемой памяти
    */
    IPC_handle IPC_createSharedMemory(const IPC_str *name, int size);

    //! Функция создает именованный объект разделяемой памяти
    /*!
    \param name - имя объекта разделяемой памяти
    \param size - размер разделяемой памяти
    \param alreadyCreated - флаг устанавливается, если память была создана
    */
    IPC_handle IPC_createSharedMemoryEx(const IPC_str *name, int size, int *alreadyCreated);

    //! Функция отображает объект разделяемой памяти в адресное пространство процесса
    /*!
    \param handle - дескриптор разделяемой памяти
    \param size - размер отображения
    */
    void* IPC_mapSharedMemory(const IPC_handle handle, unsigned size);

    //! Функция удаляет отображение разделяемой памяти
    /*!
    \param handle - дескриптор разделяемой памяти
    \param size - размер отображения
    */
    int IPC_unmapSharedMemory(const IPC_handle handle, unsigned size);

    //! Функция удаляет именованный объект разделяемой памяти
    /*!
    \param handle - дескриптор разделяемой памяти
    */
    int IPC_deleteSharedMemory(IPC_handle handle);
}

#endif //__SHMIPC_H__

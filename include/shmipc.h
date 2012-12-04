
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
	\return в случае ошибки возвращает NULL
    */
GIPCY_API    IPC_handle IPC_createSharedMemory(const IPC_str *name, int size);

    //! Функция создает именованный объект разделяемой памяти
    /*!
    \param name - имя объекта разделяемой памяти
    \param size - размер разделяемой памяти
    \param alreadyCreated - флаг устанавливается, если память была создана
	\return в случае ошибки возвращает NULL
    */
GIPCY_API    IPC_handle IPC_createSharedMemoryEx(const IPC_str *name, int size, int *alreadyCreated);

	//! Функция открывает именованный объект разделяемой памяти
    /*!
    \param name - имя объекта разделяемой памяти
    */
	GIPCY_API    IPC_handle IPC_openSharedMemory(const IPC_str *name);

    //! Функция отображает объект разделяемой памяти в адресное пространство процесса
    /*!
    \param handle - дескриптор разделяемой памяти
    \param size - размер отображения
	\return в случае ошибки возвращает NULL
    */
GIPCY_API    void* IPC_mapSharedMemory(const IPC_handle handle);

    //! Функция удаляет отображение разделяемой памяти
    /*!
    \param handle - дескриптор разделяемой памяти
    \param size - размер отображения
	\return в случае ошибки возвращает IPC_INVALID_HANDLE или IPC_GENERAL_ERROR
    */
GIPCY_API    int IPC_unmapSharedMemory(const IPC_handle handle);

    //! Функция удаляет именованный объект разделяемой памяти
    /*!
    \param handle - дескриптор разделяемой памяти
	\return в случае ошибки возвращает IPC_INVALID_HANDLE или IPC_GENERAL_ERROR
    */
GIPCY_API    int IPC_deleteSharedMemory(IPC_handle handle);
}

#endif //__SHMIPC_H__

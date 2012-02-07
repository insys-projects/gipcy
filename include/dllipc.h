
#ifndef __DLLIPC_H__
#define __DLLIPC_H__

#ifndef __IPCLIB_H__
    #include "ipclib.h"
#endif

//----------------------------------------------------------------------
// Функции для работы с разделяемыми библиотеками
//----------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

    //! Функция открывает разделяемую библиотеку c дополнительными параметрами
    /*!
    \param name - имя библиотеки
    \param param - дополнительный параметр
	\return в случае ошибки возвращает NULL
    */
GIPCY_API	IPC_handle IPC_openLibraryEx(const IPC_str *baseName, unsigned param);

    //! Функция открывает разделяемую библиотеку (динамическую библиотеку)
    /*!
    \param name - имя библиотеки
    \param param - дополнительный параметр
	\return в случае ошибки возвращает NULL
    */
GIPCY_API	IPC_handle IPC_openLibrary(const IPC_str *name, unsigned param);

    //! Функция возвращает адрес точки входа в функцию, находящуюся в разделяемой библиотеке
    /*!
    \param handle - дескриптор разделяемой библиотеки
    \param entryName - имя точки входа
	\return в случае ошибки возвращает NULL
    */
GIPCY_API    void* IPC_getEntry(IPC_handle handle, const char *entryName);

    //! Функция закрывает разделяемую библиотеку
    /*!
    \param handle - дескриптор разделяемой библиотеки
	\return в случае ошибки возвращает IPC_INVALID_HANDLE или IPC_GENERAL_ERROR
    */
GIPCY_API    int IPC_closeLibrary(IPC_handle handle);

    //! Функция возвращает описание последней ошибки, возникшей в разделяемой библиотеке (только LINUX)
GIPCY_API    char* IPC_libraryError(void);

#ifdef __cplusplus
};
#endif

#endif //__DLLIPC_H__


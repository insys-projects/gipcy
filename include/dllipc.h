
#ifndef __DLLIPC_H__
#define __DLLIPC_H__

#ifndef __IPCLIB_H__
    #include "ipclib.h"
#endif

//----------------------------------------------------------------------
// Функции для работы с разделяемыми библиотеками
//----------------------------------------------------------------------

extern "C" {

    //! Функция открывает разделяемую библиотеку c дополнительными параметрами
    /*!
    \param name - имя библиотеки
    \param param - дополнительный параметр
    */
    IPC_handle IPC_openLibraryEx(const IPC_str *baseName, unsigned param);

    //! Функция открывает разделяемую библиотеку (динамическую библиотеку)
    /*!
    \param name - имя библиотеки
    \param param - дополнительный параметр
    */
    IPC_handle IPC_openLibrary(const IPC_str *name, unsigned param);

    //! Функция возвращает адрес точки входа в функцию находящуюся в разделяемой библиотеке
    /*!
    \param handle - дескриптор разделяемой библиотеки
    \param entryName - имя точки входа
    */
    void* IPC_getEntry(IPC_handle handle, const char *entryName);

    //! Функция закрывает разделяемую библиотеку
    /*!
    \param handle - дескриптор разделяемой библиотеки
    */
    int IPC_closeLibrary(IPC_handle handle);

    //! Функция возвращает описание последней ошибки возникшей в разделяемой библиотеке
    char* IPC_libraryError(void);

};

#endif //__DLLIPC_H__


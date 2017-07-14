
#ifndef __FILEIPC_H__
#define __FILEIPC_H__

#ifndef __IPCLIB_H__
    #include "ipclib.h"
#endif

//----------------------------------------------------------------------
// Флаги для работы с файловыми функциями
//----------------------------------------------------------------------

enum IPC_flags {

    IPC_CREATE_FILE = 0x1,     //!< Всегда создает файл
    IPC_OPEN_FILE   = 0x2,     //!< Открывает файл  (если не существует, то создается)
    IPC_FILE_RDONLY = 0x10,    //!< Открывает файл в режиме только для чтения (если не существует, то ошибка)
    IPC_FILE_WRONLY = 0x20,    //!< Открывает файл в режиме только для записи (если не существует, то создается)
    IPC_FILE_RDWR   = 0x40,    //!< Открывает файл в режиме записи/чтения (если не существует, то создается)
    IPC_FILE_DIRECT = 0x80     //!< Открывает файл в режиме прямого записи/чтения (Linux)
};

enum IPC_posMethod {

    IPC_FILE_BEG =	0,    //!< позиция файла с начала файла
    IPC_FILE_CUR =	1,    //!< позиция файла с текущей позиции
    IPC_FILE_END =	2    //!< позиция файла от конца файла
};

enum IPC_Attribute {

    IPC_FILE_NORMAL		=	0,    //!< for WINDOWS - FILE_ATTRIBUTE_NORMAL
    IPC_FILE_NOBUFFER	=	1,    //!< for WINDOWS - FILE_FLAG_NO_BUFFERING
    IPC_FILE_WRTHROUGH	=	2    //!< for WINDOWS - FILE_FLAG_WRITE_THROUGH
};

//----------------------------------------------------------------------
// Функции для работы с системными вызовами
//----------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

    //! Функция открывает файл
    /*!
    \param name - имя файла или файла устройства
    \param flags - флаги
    \return дескриптор IPC или NULL
    */
GIPCY_API    IPC_handle IPC_openFile(const IPC_str *name, int flags);

    //! Функция открывает файл с дополнительными параметрами
    /*!
    \param name - имя файла или файла устройства
    \param flags - флаги
    \return дескриптор IPC или NULL
    */
GIPCY_API    IPC_handle IPC_openFileEx(const IPC_str *name, int flags, int attr);

    //! Функция закрывает файл
    /*!
    \param handle - дескриптор IPC
    \return код ошибки
    */
GIPCY_API    int IPC_closeFile(IPC_handle handle);

    //! Функция чтения из файла
    /*!
    \param handle - дескриптор файла
    \param data - указатель на буфер данных
    \param size - размер буфера данных
    \return количество прочитанных байт
    */
GIPCY_API    int IPC_readFile(IPC_handle handle, void *data, int size);

    //! Функция записи в файл
    /*!
    \param handle - дескриптор IPC
    \param data - указатель на буфер данных
    \param size - размер буфера данных
    \return количество записанных байт
    */
GIPCY_API    int IPC_writeFile(IPC_handle handle, void *data, int size);

    //! Функция установки позиции в файле
    /*!
    \param handle - дескриптор IPC
    \param pos - новая позиция в файле
    \param method - способ получения новой позиции
    \return код ошибки
    */
GIPCY_API	int IPC_setPosFile(IPC_handle handle, int pos, int method);

    //! Функция для получения размера файла
    /*!
    \param handle - дескриптор IPC
    \param size - размер файла
    \return код ошибки
    */
GIPCY_API int IPC_getFileSize(IPC_handle handle, long long* size);

#ifdef __cplusplus
}
#endif

#endif //__SYSIPC_H__

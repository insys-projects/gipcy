
#ifndef __SYSIPC_H__
#define __SYSIPC_H__

#ifndef __IPCLIB_H__
    #include "ipclib.h"
#endif

//----------------------------------------------------------------------
// Флаги для работы с системными вызовами
//----------------------------------------------------------------------

enum IPC_flags {

    IPC_CREATE_FILE = 0x1,    //!< Всегда создает файл
    IPC_OPEN_FILE   = 0x2,    //!< Открывает файл  (если не существует - ошибка??!!)
    IPC_FILE_RDONLY = 0x10,    //!< Открывает файл в режиме только для чтения
    IPC_FILE_WRONLY = 0x20,    //!< Открывает файл в режиме только для записи
    IPC_FILE_RDWR   = 0x40    //!< Открывает файл в режиме записи/чтения
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

extern "C" {

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

    //! Выполняет задержку в миллисекундах
    /*!
      \param ms - время задержки, в мс
      */
GIPCY_API    void IPC_delay(int ms);

    //! Возвращает код последней ошибки
GIPCY_API    int IPC_sysError();

    //! Функция определяет полный путь у указанному файлу
    /*!
    \param name - имя файла
    \param path - полный путь
    \return код ошибки
    */
GIPCY_API    int IPC_getFullPath(const IPC_str *name, IPC_str *path);

    //! Функция определяет текущий рабочий каталог
    /*!
    \param buf - буфер для сохранения пути к текущему каталогу
    \param size - размер буфера
    \return путь к текущей директории
    */
GIPCY_API    const IPC_str* IPC_getCurrentDir(IPC_str *buf, int size);

GIPCY_API    int IPC_getPrivateProfileString( const IPC_str *lpAppName, const IPC_str *lpKeyName, const IPC_str *lpDefault,
                                     IPC_str *lpReturnedString, int nSize, const IPC_str *lpFileName );

GIPCY_API    int IPC_writePrivateProfileString( const IPC_str *lpAppName, const IPC_str *lpKeyName, const IPC_str *lpString, const IPC_str *lpFileName );

GIPCY_API    long IPC_interlockedDecrement(volatile long *val );
GIPCY_API    long IPC_interlockedIncrement(volatile long *val );
GIPCY_API    long IPC_interlockedCompareExchange(volatile long *dst, long val, long param );
GIPCY_API    long IPC_interlockedExchange(volatile long *dst, long val );
GIPCY_API    long IPC_interlockedExchangeAdd(volatile long *dst, long val );

#ifdef _WIN32
    typedef int IPC_tls_key;
#else
    typedef pthread_key_t IPC_tls_key;
#endif
GIPCY_API    IPC_tls_key IPC_createTlsKey(void);
GIPCY_API    void* IPC_tlsGetValue(IPC_tls_key key);
GIPCY_API    int IPC_tlsSetValue(IPC_tls_key key, void *ptr);
GIPCY_API    int IPC_deleteTlsKey(IPC_tls_key key);

	//! Конвертирует дескриптор IPC в файловый дескриптор
GIPCY_API    int IPC_handleToFile(IPC_handle handle);

    //! Конвертирует дескриптор IPC в файловый дескриптор устройства
GIPCY_API    int IPC_handleToDevice(IPC_handle handle);
}

#endif //__SYSIPC_H__

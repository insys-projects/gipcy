
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

#ifdef __linux__
#define DIR_DELIMITER "/"
#else
#define DIR_DELIMITER _BRDC("\\")
#endif

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
    IPC_handle IPC_openFile(const IPC_str *name, int flags);

    //! Функция открывает файл с дополнительными параметрами
    /*!
    \param name - имя файла или файла устройства
    \param flags - флаги
    \return дескриптор IPC или NULL
    */
    IPC_handle IPC_openFileEx(const IPC_str *name, int flags, ...);

    //! Функция закрывает файл
    /*!
    \param handle - дескриптор IPC
    \return код ошибки
    */
    int IPC_closeFile(IPC_handle handle);

    //! Функция чтения из файла
    /*!
    \param handle - дескриптор файла
    \param data - указатель на буфер данных
    \param size - размер буфера данных
    \return количество прочитанных байт
    */
    int IPC_readFile(IPC_handle handle, void *data, int size);

    //! Функция записи в файл
    /*!
    \param handle - дескриптор IPC
    \param data - указатель на буфер данных
    \param size - размер буфера данных
    \return количество записанных байт
    */
    int IPC_writeFile(IPC_handle handle, void *data, int size);

    //! Функция установки позиции в файле
    /*!
    \param handle - дескриптор IPC
    \param pos - новая позиция в файле
    \param method - способ получения новой позиции
    \return код ошибки
    */
	int IPC_setPosFile(IPC_handle handle, int pos, int method);

    //! Функция открывает устройство
    /*!
    \param devname - полное имя устройства в данной ОС (возвращаемый параметр)
    \param mainname - основная часть имени устройства
    \param devnum - номер устройства
    \return дескриптор IPC или NULL
    */
    IPC_handle IPC_openDevice(IPC_str *devname, const IPC_str *mainname, int devnum);

    //! Функция закрывает устройство
    /*!
    \param handle - дескриптор IPC
    \return код ошибки
    */
    int IPC_closeDevice(IPC_handle handle);

    //! Функция чтения из устройства
    /*!
    \param handle - дескриптор IPC
    \param data - указатель на буфер данных
    \param size - размер буфера данных
    \return количество прочитанных байт
    */
    int IPC_readDevice(IPC_handle handle, void *data, int size);

    //! Функция записи в устройство
    /*!
    \param handle - дескриптор IPC
    \param data - указатель на буфер данных
    \param size - размер буфера данных
    \return количество записанных байт
    */
    int IPC_writeDevice(IPC_handle handle, void *data, int size);

    //! Описывает параметры для команд управления устройством
    struct ioctl_param {
        void *srcBuf;       //!< буфер с данными для устройства (через него передаются данные В драйвер нулевого кольца)
        int srcSize;        //!< размер буфера с данными для устройства
        void *dstBuf;       //!< буфер с данными от устройства  (через него передаются данные ИЗ драйвера нулевого кольца)
        int dstSize;        //!< dstSize - размер буфера с данными от устройства
    };

    //! Функция управления устройством
    /*!
    \param handle - дескриптор IPC
    \param cmd - код команды
    \param srcBuf - буфер с данными для устройства (через него передаются данные В драйвер нулевого кольца)
    \param srcSize - размер буфера с данными для устройства
    \param dstBuf - буфер с данными от устройства  (через него передаются данные ИЗ драйвера нулевого кольца)
    \param dstSize - размер буфера с данными от устройства
    \return код ошибки
    */
    int IPC_ioctlDevice(IPC_handle handle, unsigned long cmd, void *srcBuf, int srcSize, void *dstBuf, int dstSize);

    //! Устаревшая функция управления устройством
    int IPC_ioctlDev(IPC_handle handle, unsigned long cmd, void *srcBuf, int srcSize, void *dstBuf, int dstSize, void *overlap);

    //! Выполняет задержку в миллисекундах
    /*!
      \param ms - время задержки, в мс
      */
    void IPC_delay(int ms);

    //! Возвращает код последней ошибки
    int IPC_sysError();

    //! Функция определяет полный путь у указанному файлу
    /*!
    \param name - имя файла
    \param path - полный путь
    \return код ошибки
    */
    int IPC_getFullPath(const IPC_str *name, IPC_str *path);

    //! Функция определяет текущий рабочий каталог
    /*!
    \param buf - буфер для сохранения пути к текущему каталогу
    \param size - размер буфера
    \return путь к текущей директории
    */
    const IPC_str* IPC_getCurrentDir(IPC_str *buf, int size);

    int IPC_getPrivateProfileString( const IPC_str *lpAppName, const IPC_str *lpKeyName, const IPC_str *lpDefault,
                                     IPC_str *lpReturnedString, int nSize, const IPC_str *lpFileName );

    int IPC_writePrivateProfileString( const IPC_str *lpAppName, const IPC_str *lpKeyName, const IPC_str *lpString, const IPC_str *lpFileName );

    long IPC_interlockedDecrement(volatile long *val );
    long IPC_interlockedIncrement(volatile long *val );
    long IPC_interlockedCompareExchange(volatile long *dst, long val, long param );
    long IPC_interlockedExchange(volatile long *dst, long val );
    long IPC_interlockedExchangeAdd(volatile long *dst, long val );

#ifdef _WIN32
    typedef int IPC_tls_key;
#else
    typedef pthread_key_t IPC_tls_key;
#endif
    IPC_tls_key IPC_createTlsKey(void);
    void* IPC_tlsGetValue(IPC_tls_key key);
    int IPC_tlsSetValue(IPC_tls_key key, void *ptr);
    int IPC_deleteTlsKey(IPC_tls_key key);

    //! Конвертирует дескриптор IPC в файловый дескриптор
    int IPC_handleToFile(IPC_handle handle);

    //! Конвертирует дескриптор IPC в файловый дескриптор устройства
    int IPC_handleToDevice(IPC_handle handle);
}

#endif //__SYSIPC_H__

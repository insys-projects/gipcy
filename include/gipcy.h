#ifndef __GIPCY_H__
#define __GIPCY_H__

//----------------------------------------------------------------------

#include <stdarg.h>

#ifdef __IPC_LINUX__
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/mman.h>
#endif

#ifdef __IPC_WIN__
#include <windows.h>
#include <winioctl.h>
#include <conio.h>
#include <crtdbg.h>
#endif

//! IPC_handle - внешний тип данных используемый для работы с объектами IPC
typedef void* IPC_handle;

//----------------------------------------------------------------------
// Описание типов IPC реализованных в библиотеке
//----------------------------------------------------------------------

enum IPC_type {

    IPC_typeSemaphore,
    IPC_typeMutex,
    IPC_typeEvent,
    IPC_typeThread,
    IPC_typeSharedMem,
    IPC_typeLibrary,
    IPC_typeFile,
    IPC_typeDevice
};

//----------------------------------------------------------------------
// Функции и типы для работы со строками
//----------------------------------------------------------------------

//! Определим тип указателя на строку,
//! чтобы исключить директивы препроцессора
//! в интерфейсных функциях

#ifdef __linux__
    typedef char           IPC_str;
    #define DIR_DELIMITER "/"
#else
    #ifdef _WIN64
        typedef wchar_t    IPC_str;
        #define DIR_DELIMITER L"\\"
    #else
        typedef char       IPC_str;
        #define DIR_DELIMITER "\\"
    #endif
#endif

//----------------------------------------------------------------------
// Функции для обработки ошибок в библиотеке gipcy
//----------------------------------------------------------------------

enum IPC_error {

    IPC_ok,
    IPC_invalidHandle,
    IPC_generalError,
    IPC_noMemory,
    IPC_timeout,
    IPC_interrupted,
    IPC_overflow,
    IPC_abandoned
};

extern "C" {
    void IPC_errorString(IPC_error err);
}

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

//----------------------------------------------------------------------
// Функции для работы с событиями
//----------------------------------------------------------------------

extern "C" {

    //! Функция создает событие
    /*!
    \param name - имя события
    \param value - начальное значение события
    */
    IPC_handle IPC_createEvent(const IPC_str *name, bool manual, bool value);

    //! Функция ожидает перехода события в сигнальное состояние
    /*!
    \param handle - дескриптор события
    \param timeout - время ожидания операции
    */
    int IPC_waitEvent(const IPC_handle handle, int timeout);

    //! Функция переводит событие в сигнальное состояние
    /*!
    \param handle - дескриптор события
    */
    int IPC_setEvent(const  IPC_handle handle);

    //! Функция переводит событие в несигнальное состояние
    /*!
    \param handle - дескриптор события
    */
    int IPC_resetEvent(const  IPC_handle handle);

    //! Функция возвращает win-дескриптор события
    /*!
    \param handle - дескриптор события
    */
	//HANDLE IPC_getEvent(const  IPC_handle handle);

    //! Функция удаляет объект события
    /*!
    \param handle - дескриптор события
    */
    int IPC_deleteEvent(IPC_handle handle);
}

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

//----------------------------------------------------------------------
// Функции для работы с семаформаи
//----------------------------------------------------------------------

extern "C" {

    //! Функция создает именованный семафор
    /*!
    \param name - имя семафора
    \param value - начальное значение семафора
    */
    IPC_handle IPC_createSemaphore(const IPC_str *name, int value);

    //! Функция создает именованный семафор с дополнительным параметром
    /*!
    \param name - имя семафора
    \param value - начальное значение семафора
    */
    IPC_handle IPC_createSemaphoreEx(const IPC_str *name, int value, ... );

    //! Функция захватывает семафор или ждет его освобождения
    /*!
    \param handle - дескриптор семафора
    \param timeout - время ожидания операции
    */
    int IPC_lockSemaphore(const IPC_handle handle, int timeout);

    //! Функция освобождает занятый семафор
    /*!
    \param handle - дескриптор семафора
    */
    int IPC_unlockSemaphore(const IPC_handle handle);

    //! Функция удаляет именованный объект семафора
    /*!
    \param handle - дескриптор семафора
    */
    int IPC_deleteSemaphore(IPC_handle handle);
}

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
    void* IPC_mapSharedMemory(const IPC_handle handle);

    //! Функция удаляет отображение разделяемой памяти
    /*!
    \param handle - дескриптор разделяемой памяти
    \param size - размер отображения
    */
    int IPC_unmapSharedMemory(const IPC_handle handle);

    //! Функция удаляет именованный объект разделяемой памяти
    /*!
    \param handle - дескриптор разделяемой памяти
    */
    int IPC_deleteSharedMemory(IPC_handle handle);
}

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
    IPC_handle IPC_openFile(const IPC_str *name, int flags);

    //! Функция открывает файл с дополнительными параметрами
    /*!
    \param name - имя файла или файла устройства
    \param flags - флаги
    \return дескриптор IPC или NULL
    */
    IPC_handle IPC_openFileEx(const IPC_str *name, int flags, int attr);

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

    //! Инициализирует буфер клавиатуры в неканоническое состояние
    void IPC_initKeyboard(void);

    //! Восстанавливает буфер клавиатуры в каноническое состояние
    void IPC_cleanupKeyboard(void);

    //! Ожидает нажатия клавиши
    int IPC_getch(void);

    //! Ожидает нажатия клавиши
    int IPC_kbhit(void);
}

//----------------------------------------------------------------------
// Функции для работы с потоками
//----------------------------------------------------------------------

#define CREATE_THREAD_STARTED   0x0
#define CREATE_THREAD_PAUSED    0x1
#define THREAD_START            0x2
#define THREAD_STOP             0x4
#define THREAD_RESUME           0x8
#define THREAD_EXIT             0x10

#ifdef __linux__
    #define __IPC_API
    typedef void* thread_value;                          //!< Тип значения, возвращаемого функцией потока при завершении
#else
    #define __IPC_API __stdcall
    typedef unsigned int thread_value;                   //!< Тип значения, возвращаемого функцией потока при завершении
#endif

typedef thread_value (__IPC_API thread_func) (void*);    //!< Тип указателя на функцию потока (зависит от операционной системы)

//! Структура передаваемая функции потока в качестве параметра (зависит от операционной системы)
struct thread_param {

    thread_func*    threadFunction;      //!< Указатель на функцию потока (user input)
    void*           threadData;          //!< Данные передаваемые для функции потока (user input)
    IPC_handle      threadMutex;         //!< Мьютекс на котором поток будет ожидать команд управления (user input)
    thread_value    threadExitValue;     //!< Значение возвращенное функцией потока (output)
};


extern "C" {

    //! Функция создает поток
    /*!
    \param name - имя потока
    \param function - указатель на рабочую функцию потока
    \param param - параметры передаваемые рабочей функции потока
    \param flags - флаги потока
    */
    IPC_handle IPC_createThread(const IPC_str *name, thread_func* function, void* param);

    //! Функция создает поток
    /*! Для синхронизации действий с потоком исрользовать семафор
    \param name - имя потока
    \param tp - парметры для потока, заполняются пользователем
    \param flags - флаги потока
    */
    IPC_handle IPC_createThreadEx(const IPC_str *name, struct thread_param *tp, int flags);

    //! Функция разрешает потоку выполняться
    /*!
    \param handle - дескриптор потока
    */
    int IPC_startThread(const IPC_handle handle);

    //! Функция приостанавливает выполнение потока
    /*!
    \param handle - дескриптор потока
    */
    int IPC_stopThread(const IPC_handle handle);

    //! Функция ожидает завершения потока
    /*!
    \param handle - дескриптор потока
    \param timeout - время ожидания в мс
    */
    int IPC_waitThread(const IPC_handle handle, int timeout);

    //! Функция останавливает поток и удаляет дескриптор потока
    /*!
    \param handle - дескриптор разделяемой памяти
    */
    int IPC_deleteThread(IPC_handle handle);
};

#endif //__GIPCY_H__

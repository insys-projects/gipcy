
#ifndef __SYSIPC_H__
#define __SYSIPC_H__

#ifndef __IPCLIB_H__
    #include "ipclib.h"
#endif

//----------------------------------------------------------------------
// Функции для работы с системными вызовами
//----------------------------------------------------------------------

extern "C" {

    //! Функция открывает файл
    /*!
    \param name - имя файла или файла устройства
    \param flags - флаги
    \param mode - режим
    \return дескриптор IPC или NULL
    */
    IPC_handle IPC_openFile(const IPC_str *name, int flags, int mode);

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

    //! Функция управления устройством
    /*!
    \param handle - дескриптор IPC
    \param cmd - код команды
    \param param - параметры команды
    \return код ошибки
    */
    int IPC_ioctlDevice(IPC_handle handle, unsigned long cmd, void *param);

    //! Выполняет задержку в миллисекундах
    /*!
      \param ms - время задержки, в мс
      */
    void IPC_delay(int ms);

    //! Возвращает код последней ошибки
    int IPC_sysError();
}

#endif //__SYSIPC_H__

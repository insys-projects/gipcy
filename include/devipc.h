
#ifndef __DEVIPC_H__
#define __DEVIPC_H__

#ifndef __IPCLIB_H__
    #include "ipclib.h"
#endif

//----------------------------------------------------------------------
// Функции для работы с устройствами
//----------------------------------------------------------------------

extern "C" {

    //! Функция открывает устройство
    /*!
    \param devname - полное имя устройства в данной ОС (возвращаемый параметр)
    \param mainname - основная часть имени устройства
    \param devnum - номер устройства
    \return дескриптор IPC или NULL
    */
GIPCY_API    IPC_handle IPC_openDevice(IPC_str *devname, const IPC_str *mainname, int devnum);

    //! Функция закрывает устройство
    /*!
    \param handle - дескриптор IPC
    \return код ошибки
    */
GIPCY_API    int IPC_closeDevice(IPC_handle handle);

    //! Функция чтения из устройства
    /*!
    \param handle - дескриптор IPC
    \param data - указатель на буфер данных
    \param size - размер буфера данных
    \return количество прочитанных байт
    */
GIPCY_API    int IPC_readDevice(IPC_handle handle, void *data, int size);

    //! Функция записи в устройство
    /*!
    \param handle - дескриптор IPC
    \param data - указатель на буфер данных
    \param size - размер буфера данных
    \return количество записанных байт
    */
GIPCY_API    int IPC_writeDevice(IPC_handle handle, void *data, int size);

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
GIPCY_API   int IPC_ioctlDevice(IPC_handle handle, unsigned long cmd, void *srcBuf, int srcSize, void *dstBuf, int dstSize);

    //! Устаревшая функция управления устройством
GIPCY_API   int IPC_ioctlDeviceOvl(IPC_handle handle, unsigned long cmd, void *srcBuf, int srcSize, void *dstBuf, int dstSize, void *overlap);

    //! Функция отображения физической памяти на виртуальную память процесса (нужна только под LINUX)
GIPCY_API   int IPC_mapPhysAddr(IPC_handle handle, void** virtAddr, size_t physAddr, unsigned long size);

    //! Функция отключает отображение физической памяти на виртуальную память процесса (нужна только под LINUX)
GIPCY_API   int IPC_unmapPhysAddr(IPC_handle handle, void* virtAddr, unsigned long size);

#ifdef __linux__
    //! Описывает параметры для команд управления устройством
    struct ioctl_param {
        void    *srcBuf;       //!< буфер с данными для устройства (через него передаются данные В драйвер нулевого кольца)
        size_t  srcSize;        //!< размер буфера с данными для устройства
        void    *dstBuf;       //!< буфер с данными от устройства  (через него передаются данные ИЗ драйвера нулевого кольца)
        size_t  dstSize;        //!< dstSize - размер буфера с данными от устройства
    } __attribute__((packed));
#endif

}



#endif //__DEVIPC_H__

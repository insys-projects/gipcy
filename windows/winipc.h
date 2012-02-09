
#ifndef __WINIPC_H__
#define __WINIPC_H__

#ifndef __GIPCY_H__
    #include "gipcy.h"
#endif
//#ifndef __IPCTYPE_H__
//    #include "ipctype.h"
//#endif
//
//#ifndef __STRIPC_H__
//    #include "stripc.h"
//#endif

//#include <windows.h>

//! Структура описывает внутренний тип struct ipc_t используемый библиотекой IPC
/*!
    struct ipc_t - является внутренним типом данных, используемый библиотекой IPC,
    для представления любого из объектов синхронизации в операционной системе Windows
*/
struct ipc_t {

    IPC_str         *ipc_name;          //!< Имя объекта IPC
    IPC_type         ipc_type;          //!< Тип объекта IPC
    HANDLE           ipc_descr;         //!< Дескриптор объекта IPC (зависит от платформы)
    void            *ipc_data;          //!< Данные объекта IPC (опционально)
    size_t           ipc_size;          //!< Размер блока данных объекта IPC (опционально)
};

typedef struct ipc_t* ipc_handle_t;

ipc_handle_t allocate_ipc_object(const IPC_str *name, IPC_type type);
void delete_ipc_object(ipc_handle_t h);

#endif //__WINIPC_H__

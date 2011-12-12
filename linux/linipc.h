
#ifndef __LINIPC_H__
#define __LINIPC_H__

#ifndef __IPCTYPE_H__
    #include "ipctype.h"
#endif

#include <stdint.h>
#include <dlfcn.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/sem.h>

union ipc {
    int          ipc_file;      //!< Используется если IPC файл в ФС
    int          ipc_dev;       //!< Используется если IPC файл устройства
    int          ipc_sem;       //!< Используется если IPC семафор SYSTEM V
    int          ipc_shm;       //!< Используется если IPC разделяемая память POSIX
    pthread_t    ipc_thread;    //!< Используется если IPC поток выполнения
    void*        ipc_lib;       //!< Используется если IPC разделяемая библиотека
};

//! Структура описывает внутренний тип struct ipc_t используемый библиотекой IPC
/*!
    struct ipc_t - это внутренний тип данных, используемый библиотекой IPC,
    для представления любого из объектов синхронизации в операционной системе Linux
*/
struct ipc_t {

    char            *ipc_name;          //!< Имя объекта IPC
    key_t            ipc_key;           //!< Ключ объекта IPC (для System V)
    IPC_type         ipc_type;          //!< Тип объекта IPC
    union ipc        ipc_descr;         //!< Дескриптор объекта IPC (зависит от платформы)
    void            *ipc_data;          //!< Данные объекта IPC (опционально)
    size_t           ipc_size;          //!< Размер блока данных объекта IPC (опционально)
};

//! Тип дескриптора объекта IPC
/*!
    ipc_handle_t есть указатель на структуру struct ipc_t
    используется для работы с объектами IPC всеми
    функциями библиотеки.
*/
typedef struct ipc_t* ipc_handle_t;

#ifdef __VERBOSE__
#define DEBUG_PRINT(fmt, args...)    fprintf(stderr, fmt, ## args)
#else
#define DEBUG_PRINT(fmt, args...)
#endif

#if 0

//! Функция возвращает имя объекта IPC
/*!
    Функция возвращает имя объекта IPC с префиксом /tmp/ipclib.
    \param name - имя объекта IPC
*/
char* create_ipc_name(const char *name);

//! Функция удаляет имя объекта IPC
/*!
    Функция освобождает память выделенную для имени объекта IPC
    \param name - имя объекта IPC
*/
void delete_ipc_name(char *name);

//! Функция создает файл объекта IPC
/*!
    Функция создает файл в файловой системе, для которого будем получать ключ key_t
    и возвращает имя созданного файла
    \param name - имя объекта IPC
*/
int create_ipc_file(const char *name);

//! Функция удаляет файл объекта IPC
/*!
    Функция удаляет файл из файловой системы
    и освобождает памть для имени файла
    \param name - имя объекта IPC
*/
int delete_ipc_file(const char *name);

//! Функция возвращает ключ key_t для указанного имени
/*!
    Функция создает файл в файловой системе, а затем для этого
    пути получает ключ key_t
    \param name - имя объекта IPC
*/
key_t get_ipc_key( const char *name );

//! Функция удаляет ключ key_t и временный файл
/*!
    Функция создает файл в файловой системе, а затем для этого
    пути получает ключ key_t
    \param ipc_key - ключ объекта IPC
    \param name - имя объекта IPC
*/
int free_ipc_key( key_t ipc_key, const char *name );

#endif

//! Функция создает дескриптор объекта IPC c заданным именем
/*!
    \param name - имя объекта IPC
*/
ipc_handle_t allocate_ipc_object(const char *name, IPC_type type);

//! Функция удаляет дескриптор объекта IPC c заданным именем
/*!
    \param h - дескриптор объекта IPC
*/
void delete_ipc_object(ipc_handle_t h);

#endif //__LINIPC_H__

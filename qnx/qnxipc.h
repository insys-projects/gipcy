
#ifndef __QNXIPC_H__
#define __QNXIPC_H__

#ifndef __GIPCY_H__
    #include "gipcy.h"
#endif

#include <stdint.h>
#include <dlfcn.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>

//-----------------------------------------------------------------------------

union ipc {
    int          ipc_file;      //!< Используется если IPC файл в ФС
    int          ipc_dev;       //!< Используется если IPC файл устройства
    sem_t        *ipc_sem;      //!< Используется если IPC семафор POSIX
    int          ipc_shm;       //!< Используется если IPC разделяемая память POSIX
    pthread_t    ipc_thread;    //!< Используется если IPC поток выполнения
    void*        ipc_lib;       //!< Используется если IPC разделяемая библиотека
};

//-----------------------------------------------------------------------------

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
    void            *ipc_user;          //!< Дополнительный параметр объекта IPC (опционально)
};

//-----------------------------------------------------------------------------

//! Тип дескриптора объекта IPC
/*!
    ipc_handle_t есть указатель на структуру struct ipc_t
    используется для работы с объектами IPC всеми
    функциями библиотеки.
*/
typedef struct ipc_t* ipc_handle_t;

//-----------------------------------------------------------------------------

#ifdef __VERBOSE__
#define DEBUG_PRINT(fmt, args...)    fprintf(stderr, fmt, ## args)
#else
#define DEBUG_PRINT(fmt, args...)
#endif

//-----------------------------------------------------------------------------

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
/* union semun is defined by including <sys/sem.h> */
#else
/* according to X/OPEN we have to define it ourselves */
//! Аргумент используемый в качестве команды для вызова semctl. Передается по значению.
union semun {
        int val;                  //!< Используется командой SETVAL
        struct semid_ds *buf;     //!< Используется командами IPC_SET и IPC_STAT
        unsigned short  *array;   //!< Используется командами GETALL и SETALL
        struct seminfo  *__buf;   //!< Специфичный для Linux команды IPC_INFO
};
#endif

#ifndef SEMVMX
#define SEMVMX 32768
#endif

#ifndef SEM_R
#define SEM_R 0x100
#endif

#ifndef SEM_A
#define SEM_A 0x80
#endif

#define SVSEM_MODE ( SEM_R | SEM_A | SEM_R>>3 | SEM_R>>6 )

//! Определяет флаги доступа к семафорам SYSTEM V
#define IPC_SVSEM_MODE   (SEM_R | SEM_A | SEM_R >> 3 | SEM_R >> 6)

//-----------------------------------------------------------------------------

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

//! Функция проверяет, можно ли удалить объект IPC (семафор, событие, мьютекс)
/*!
    \param h - дескриптор объекта IPC
*/
bool is_ok_remove(ipc_handle_t h);

//! Функция проверяет правильность дескриптора IPC
/*!
    \param h - дескриптор объекта IPC
    \param h_type - тип дескриптора объекта IPC
*/
bool chechk_handle(ipc_handle_t h, int h_type);

//! Функция открывает устройство IPC и возвращает дескриптор устройства
/*!
    \param name - имя устройства
*/
int open_ipc_driver(const char *name);

//! Функция возвращает дескриптор устройства IPC
int ipc_driver_handle(void);

//! Функция закрывает устройство IPC
/*!
    \param name - имя устройства
*/
int close_ipc_driver(int fd);

#endif //__QNXIPC_H__

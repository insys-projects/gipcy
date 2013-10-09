
#ifndef __IPCDRV_H__
#define __IPCDRV_H__

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <semaphore.h>

#include <vector>

#ifndef __IPCIOTCL_H__
    #include "ipcioctl.h"
#endif

//-----------------------------------------------------------------------------
#define SEM_ID    0x11223344
#define MUTEX_ID  0x22334455
#define EVENT_ID  0x33445566
#define SHM_ID    0x44556677
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
typedef volatile unsigned atomic_t;
//-----------------------------------------------------------------------------

//! Структура описывает внутренний тип struct ipcsem_t используемый библиотекой IPC
/*!
    struct ipcsem_t - это внутренний тип данных, используемый библиотекой IPC,
    для представления объектов синхронизации типа семафор
*/
struct ipcsem_t {

    char                    		sem_name[128];             //!< Имя семафора
    void*                   		sem_file;                  //!< Дескриптор файла устройства
    void*                   		sem_handle;                //!< Адрес этой структуры
    atomic_t                		sem_owner_count;           //!< Счетчик пользователей семафора
    atomic_t                		sem_lock_count;            //!< Счетчик блокировок семафора
    atomic_t                		sem_unlock_count;          //!< Счетчик разблокировок семафора
    sem_t							sem;                       //!< Семафор
    uint32_t                		sem_id;                    //!< Идентификатор семафора
};

//-----------------------------------------------------------------------------

//! Структура описывает внутренний тип struct ipcmutex_t используемый библиотекой IPC
/*!
    struct ipcmutex_t - это внутренний тип данных, используемый библиотекой IPC,
    для представления объектов синхронизации типа семафор
*/
struct ipcmutex_t {

    char                    		mutex_name[128];             //!< Имя мьютекса
    void*                   		mutex_file;                  //!< Дескриптор файла устройства
    void*                   		mutex_handle;                //!< Адрес этой структуры
    atomic_t                		mutex_owner_count;           //!< Счетчик пользователей мьютекса
    atomic_t                		mutex_lock_count;            //!< Счетчик блокировок мьютекса
    atomic_t                		mutex_unlock_count;          //!< Счетчик разблокировок мьютекса
    sem_t							mutex;                       //!< Семафор
    uint32_t                		mutex_id;                    //!< Идентификатор мьютекса
};

//-----------------------------------------------------------------------------

//! Структура описывает внутренний тип struct ipcevent_t используемый библиотекой IPC
/*!
    struct ipcevent_t - это внутренний тип данных, используемый библиотекой IPC,
    для представления объектов синхронизации типа семафор
*/
struct ipcevent_t {

    char                    		event_name[128];             //!< Имя события
    void*                   		event_file;                  //!< Дескриптор файла устройства
    void*                   		event_handle;                //!< Адрес этой структуры
    atomic_t                		event_owner_count;           //!< Счетчик пользователей события
    atomic_t                		event_lock_count;            //!< Счетчик блокировок события
    atomic_t                		event_unlock_count;          //!< Счетчик разблокировок события
    sem_t							event;                       //!< Дескриптор события
    uint32_t                		event_flag;                  //!< Флаг автосброса
    uint32_t                		event_id;                    //!< Идентификатор события
};

//-----------------------------------------------------------------------------

//! Структура описывает внутренний тип struct ipcsem_t используемый библиотекой IPC
/*!
    struct ipcsem_t - это внутренний тип данных, используемый библиотекой IPC,
    для представления объектов синхронизации типа семафор
*/
struct ipcshm_t {

    char                    		shm_name[128];             //!< Имя разделяемой памяти
    void*                   		shm_file;                  //!< Дескриптор файла устройства
    void*                   		shm_handle;                //!< Адрес этой структуры
    atomic_t                		shm_owner_count;           //!< Счетчик пользователей разделяемой памяти
    uint32_t                		shm_id;                    //!< Идентификатор разделяемой памяти
};

//-----------------------------------------------------------------------------

struct ipc_driver {

    dev_t                   		m_devno;
    char                    		m_name[128];
    sem_t							m_ipc_mutex;
    sem_t							m_ipc_sem;
    atomic_t                		m_usage;
    int                     		m_index;

	std::vector<struct ipcsem_t*>	m_sem_list;
	sem_t							m_sem_lock;

	std::vector<struct ipcmutex_t*>	m_mutex_list;
	sem_t							m_mutex_lock;

	std::vector<struct ipcevent_t*>	m_event_list;
	sem_t							m_event_lock;

	std::vector<struct ipcshm_t*>	m_shm_list;
	sem_t							m_shm_lock;
};

//-----------------------------------------------------------------------------

extern int dbg_trace;
extern int err_trace;

#ifndef PRINTK
#define PRINTK(S...) fprintf(stderr, S)
#endif

#define dbg_msg(flag, S...) do { if(flag) PRINTK("IPCDRV: " S); } while(0)
#define err_msg(flag, S...) do { if(flag) PRINTK("IPCDRV: " S); } while(0)

//-----------------------------------------------------------------------------

void* ipc_sem_create( struct ipc_driver *drv, struct ipc_create_t *param );
int ipc_sem_lock( struct ipc_driver *drv, struct ipc_lock_t *param );
int ipc_sem_unlock( struct ipc_driver *drv, struct ipc_unlock_t *param );
int ipc_sem_close( struct ipc_driver *drv, struct ipc_close_t *param );
int ipc_sem_close_all( struct ipc_driver *drv );

void* ipc_mutex_create( struct ipc_driver *drv, struct ipc_create_t *param );
int ipc_mutex_lock( struct ipc_driver *drv, struct ipc_lock_t *param );
int ipc_mutex_unlock( struct ipc_driver *drv, struct ipc_unlock_t *param );
int ipc_mutex_close( struct ipc_driver *drv, struct ipc_close_t *param );
int ipc_mutex_close_all( struct ipc_driver *drv );

void* ipc_event_create( struct ipc_driver *drv, struct ipc_create_t *param );
int ipc_event_lock( struct ipc_driver *drv, struct ipc_lock_t *param );
int ipc_event_unlock( struct ipc_driver *drv, struct ipc_unlock_t *param );
int ipc_event_reset( struct ipc_driver *drv, struct ipc_reset_t *param );
int ipc_event_close( struct ipc_driver *drv, struct ipc_close_t *param );
int ipc_event_close_all( struct ipc_driver *drv );

void* ipc_shm_open( struct ipc_driver *drv, struct ipc_create_t *param );
int ipc_shm_close( struct ipc_driver *drv, struct ipc_close_t *param );
int ipc_shm_close_all( struct ipc_driver *drv );

//-----------------------------------------------------------------------------

struct ipc_driver* create_instance(int index);
void remove_instance(struct ipc_driver* entry);
struct timespec ms_to_timespec(int ms);

//-----------------------------------------------------------------------------

#endif //__IPCDRV_H__

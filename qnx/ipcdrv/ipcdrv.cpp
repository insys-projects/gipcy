
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/neutrino.h>
#include <sys/resmgr.h>

#include "ipcdrv.h"

//-----------------------------------------------------------------------------

struct ipc_driver* create_instance(int index)
{
	struct  ipc_driver* entry = (struct  ipc_driver*)malloc(sizeof(struct ipc_driver));
	if(!entry) {
		fprintf(stderr, "Error allocate memory for ipc_driver instance\n");
		return 0;
	}
	
	memset(entry, 0, sizeof(struct ipc_driver));
	
	entry->m_index = index;
	entry->m_usage = 0;

	sem_init(&entry->m_ipc_mutex, 0, 1);
	sem_init(&entry->m_ipc_sem, 0, 1);
	sem_init(&entry->m_sem_lock, 0, 1);
	sem_init(&entry->m_event_lock, 0, 1);
	sem_init(&entry->m_mutex_lock, 0, 1);
	sem_init(&entry->m_shm_lock, 0, 1);

	entry->m_sem_list.clear();
	entry->m_mutex_list.clear();
	entry->m_event_list.clear();
	entry->m_shm_list.clear();

	snprintf(entry->m_name, sizeof(entry->m_name), "%s%d", IPC_DRIVER_NAME, entry->m_index);

	return entry;
}

//-----------------------------------------------------------------------------

void remove_instance(struct ipc_driver* entry)
{
	if(!entry)
		return;

	ipc_sem_close_all(entry);
	ipc_mutex_close_all(entry);
	ipc_event_close_all(entry);
	ipc_shm_close_all(entry);

	entry->m_sem_list.clear();
	entry->m_mutex_list.clear();
	entry->m_event_list.clear();
	entry->m_shm_list.clear();

	sem_destroy(&entry->m_ipc_mutex);
	sem_destroy(&entry->m_ipc_sem);
	sem_destroy(&entry->m_sem_lock);
	sem_destroy(&entry->m_event_lock);
	sem_destroy(&entry->m_mutex_lock);
	sem_destroy(&entry->m_shm_lock);
}

//-----------------------------------------------------------------------------

struct timespec ms_to_timespec(int ms)
{
	struct timespec tm;
	struct timespec tt;

	uint64_t ns = ms*1000*1000;
	nsec2timespec(&tt, ns);

	clock_gettime(CLOCK_REALTIME, &tm);

	tm.tv_sec += tt.tv_sec;
	tm.tv_nsec += tt.tv_nsec;

	return tm;
}

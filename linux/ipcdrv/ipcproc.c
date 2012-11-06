
#include <linux/kernel.h>
#define __NO_VERSION__
#include <linux/module.h>
#include <linux/types.h>
#include <linux/ioport.h>
#include <linux/pci.h>
#include <linux/pagemap.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <asm/io.h>

#include "ipcmodule.h"
#include "ipcproc.h"

//--------------------------------------------------------------------

void ipc_register_proc( char *name, void *fptr, void *data )
{
	create_proc_read_entry( name, 0, NULL, fptr, data );
}

//--------------------------------------------------------------------

void ipc_remove_proc( char *name )
{
	remove_proc_entry( name, NULL );
}

//--------------------------------------------------------------------

int ipc_proc_info(  char *buf, 
		    char **start, 
		    off_t off,
		    int count, 
		    int *eof, 
		    void *data )
{
	char *p = buf;
	struct ipc_driver *drv = (struct ipc_driver*)data;

	if(!drv) {
            p += sprintf(p,"Invalid driver pointer\n" );
	    *eof = 1;
	    return p - buf;
	}

        p += sprintf(p,"Summary:\n" );
        p += sprintf(p,"Total semaphores: %d\n", 0 );
        p += sprintf(p,"Total mutexes: %d\n", 0 );
        p += sprintf(p,"Total events: %d\n", 0 );

        p += sprintf(p,"Semaphore list:\n" );
        p += sprintf(p,"Mutex list:\n" );
        p += sprintf(p,"Event list:\n" );

        *eof = 1;

        return p - buf;
}

//--------------------------------------------------------------------

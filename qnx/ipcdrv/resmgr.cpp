
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/neutrino.h>
#include <sys/resmgr.h>

#include "ipcioctl.h"
#include "ipcdrv.h"

//-----------------------------------------------------------------------------

using namespace std;

//-----------------------------------------------------------------------------

void options (int argc, char **argv);

//-----------------------------------------------------------------------------

int io_open (resmgr_context_t *ctp, io_open_t  *msg, RESMGR_HANDLE_T *handle, void *extra);
int	io_close_ocb(resmgr_context_t *ctp, void *reserved, RESMGR_OCB_T *ocb);
int	io_close_dup(resmgr_context_t *ctp, io_close_t *msg, RESMGR_OCB_T *ocb);
int	io_devctl(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb);

//-----------------------------------------------------------------------------

resmgr_connect_funcs_t  connect_funcs;
resmgr_io_funcs_t       io_funcs;
dispatch_t              *dpp;
resmgr_attr_t           rattr;
dispatch_context_t      *ctp;
iofunc_attr_t           ioattr;
struct ipc_driver		*entry = 0;

//-----------------------------------------------------------------------------

int     optv;
int		err_trace = 1;
int		dbg_trace = 1;

//-----------------------------------------------------------------------------

volatile int exit_flag = 0;

//-----------------------------------------------------------------------------

void exit_handler(int signo)
{
	fprintf(stderr, "%s(): SIGNAL = %d\n", __FUNCTION__, signo);
	exit_flag = 1;
}

//-----------------------------------------------------------------------------

int main (int argc, char **argv)
{
	int pathID;

	signal(SIGINT, exit_handler);
	signal(SIGTERM, exit_handler);
	signal(SIGQUIT, exit_handler);

	/* Check for command line options (-v) */
	options (argc, argv);

	/* Allocate and initialize a dispatch structure for use
	 * by our main loop. This is for the resource manager
	 * framework to use. It will receive messages for us,
	 * analyze the message type integer and call the matching
	 * handler callback function (i.e. io_open, io_read, etc.) */
	dpp = dispatch_create ();
	if (dpp == NULL) {
		fprintf (stderr, "%s:  couldn't dispatch_create: %s\n",
				IPC_DRIVER_NAME, strerror (errno));
		exit (1);
	}

	/* Set up the resource manager attributes structure. We'll
	 * use this as a way of passing information to
	 * resmgr_attach(). The attributes are used to specify
	 * the maximum message length to be received at once,
	 * and the number of message fragments (iov's) that
	 * are possible for the reply.
	 * For now, we'll just use defaults by setting the
	 * attribute structure to zeroes. */
	memset (&rattr, 0, sizeof (rattr));

	/* Now, let's intialize the tables of connect functions and
	 * I/O functions to their defaults (system fallback
	 * routines) and then override the defaults with the
	 * functions that we are providing. */
	iofunc_func_init (_RESMGR_CONNECT_NFUNCS, &connect_funcs,
			_RESMGR_IO_NFUNCS, &io_funcs);
	/* Now we override the default function pointers with
	 * some of our own coded functions: */
	connect_funcs.open = io_open;
	io_funcs.close_ocb = io_close_ocb;
	io_funcs.close_dup = io_close_dup;
	io_funcs.devctl = io_devctl;


	/* Initialize the device attributes for the particular
	 * device name we are going to register. It consists of
	 * permissions, type of device, owner and group ID */
	iofunc_attr_init (&ioattr, S_IFCHR | 0666, NULL, NULL);

	/* Next we call resmgr_attach() to register our device name
	 * with the process manager, and also to let it know about
	 * our connect and I/O functions. */
	pathID = resmgr_attach (dpp, &rattr, "/dev/"IPC_DRIVER_NAME,
			_FTYPE_ANY, 0, &connect_funcs, &io_funcs, &ioattr);
	if (pathID == -1) {
		fprintf (stderr, "%s:  couldn't attach pathname: %s\n",
				IPC_DRIVER_NAME, strerror (errno));
		exit (1);
	}

	entry = create_instance(0);

	/* Now we allocate some memory for the dispatch context
	 * structure, which will later be used when we receive
	 * messages. */
	ctp = dispatch_context_alloc (dpp);

	/* Done! We can now go into our "receive loop" and wait
	 * for messages. The dispatch_block() function is calling
	 * MsgReceive() under the covers, and receives for us.
	 * The dispatch_handler() function analyzes the message
	 * for us and calls the appropriate callback function. */
	while (1) {
		if ((ctp = dispatch_block (ctp)) == NULL) {
			fprintf (stderr, "%s:  dispatch_block failed: %s\n",
					IPC_DRIVER_NAME, strerror (errno));
			if(exit_flag) {
				fprintf (stderr, "%s:  Exit flag was raised!\n", IPC_DRIVER_NAME);
			}

			remove_instance(entry);

			exit (1);
		}
		/* Call the correct callback function for the message
		 * received. This is a single-threaded resource manager,
		 * so the next request will be handled only when this
		 * call returns. Consult our documentation if you want
		 * to create a multi-threaded resource manager. */
		dispatch_handler (ctp);
	}
}

//-----------------------------------------------------------------------------

int io_open (resmgr_context_t *ctp, io_open_t *msg, RESMGR_HANDLE_T *handle, void *extra)
{
	fprintf(stderr, "%s()\n", __FUNCTION__);
	return (iofunc_open_default (ctp, msg, handle, extra));
}

//-----------------------------------------------------------------------------

int io_close_ocb(resmgr_context_t *ctp, void *reserved, RESMGR_OCB_T *ocb)
{
	fprintf(stderr, "%s()\n", __FUNCTION__);
	return iofunc_close_ocb_default(ctp, reserved, ocb);
}

//-----------------------------------------------------------------------------

int	io_close_dup(resmgr_context_t *ctp, io_close_t *msg, RESMGR_OCB_T *ocb)
{
	fprintf(stderr, "%s()\n", __FUNCTION__);
	return iofunc_close_dup_default(ctp, msg, ocb);
}

//-----------------------------------------------------------------------------

int	io_devctl(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb)
{
	int error = 0;
	int nbytes = 0;

	fprintf(stderr, "%s()\n", __FUNCTION__);

	error = iofunc_devctl_default(ctp, msg, ocb);
	if(error != (int)_RESMGR_DEFAULT) {
		return error;
	}

	int data_area_size = ctp->size - sizeof(io_devctl_t);
	int input_msg_size = msg->i.nbytes;

	fprintf(stderr, "%s(): data_area_size = %d\n", __FUNCTION__, data_area_size);
    fprintf(stderr, "%s(): input_msg_size = %d\n", __FUNCTION__, input_msg_size);

	switch(msg->i.dcmd) {

	//-----------------------------------------------------------------------------
	// Semaphores
	//
    case IOCTL_IPC_SEM_OPEN: {

        struct ipc_create_t *ipc_param = (struct ipc_create_t *)_DEVCTL_DATA(msg->i);
        ipc_param->handle = ipc_sem_create( entry, ipc_param );
        if(!ipc_param->handle) {
            err_msg(err_trace, "%s(): Error in ipc_sem_create()\n", __FUNCTION__);
            error = -EINVAL;
            return error;
        } else {
            nbytes = sizeof(struct ipc_create_t);
        }

    } break;
    case IOCTL_IPC_SEM_LOCK: {

        struct ipc_lock_t *ipc_param = (struct ipc_lock_t*)_DEVCTL_DATA(msg->i);
        error = ipc_sem_lock( entry, ipc_param );
        if(error < 0) {
            err_msg(err_trace, "%s(): Error in ipc_sem_lock()\n", __FUNCTION__);
            return error;
        }

	} break;
    case IOCTL_IPC_SEM_UNLOCK: {

        struct ipc_unlock_t *ipc_param = (struct ipc_unlock_t*)_DEVCTL_DATA(msg->i);
        error = ipc_sem_unlock( entry, ipc_param );
        if(error < 0) {
            err_msg(err_trace, "%s(): Error in ipc_sem_unlock()\n", __FUNCTION__);
            return error;
        }

    } break;
    case IOCTL_IPC_SEM_CLOSE: {

        struct ipc_close_t *ipc_param = (struct ipc_close_t *)_DEVCTL_DATA(msg->i);
        error = ipc_sem_close( entry, ipc_param );
        if(error < 0) {
            err_msg(err_trace, "%s(): Error in ipc_sem_close()\n", __FUNCTION__);
            return error;
        }

    } break;

    //-----------------------------------------------------------------------------
	// Mutexes
	//
    case IOCTL_IPC_MUTEX_OPEN: {

        struct ipc_create_t *ipc_param = (struct ipc_create_t *)_DEVCTL_DATA(msg->i);
        ipc_param->handle = ipc_mutex_create( entry, ipc_param );
        if(!ipc_param->handle) {
            err_msg(err_trace, "%s(): Error in ipc_mutex_create()\n", __FUNCTION__);
            error = -EINVAL;
            return error;
        } else {
            nbytes = sizeof(struct ipc_create_t);
        }

    } break;
    case IOCTL_IPC_MUTEX_LOCK: {

        struct ipc_lock_t *ipc_param = (struct ipc_lock_t*)_DEVCTL_DATA(msg->i);
        error = ipc_mutex_lock( entry, ipc_param );
        if(error < 0) {
            err_msg(err_trace, "%s(): Error in ipc_mutex_lock()\n", __FUNCTION__);
            return error;
        }

    } break;
    case IOCTL_IPC_MUTEX_UNLOCK: {

        struct ipc_unlock_t *ipc_param = (struct ipc_unlock_t*)_DEVCTL_DATA(msg->i);
        error = ipc_mutex_unlock( entry, ipc_param );
        if(error < 0) {
            err_msg(err_trace, "%s(): Error in ipc_mutex_lock()\n", __FUNCTION__);
            return error;
        }

    } break;
    case IOCTL_IPC_MUTEX_CLOSE: {

        struct ipc_close_t *ipc_param = (struct ipc_close_t *)_DEVCTL_DATA(msg->i);
        error = ipc_mutex_close( entry, ipc_param );
        if(error < 0) {
            err_msg(err_trace, "%s(): Error in ipc_mutex_close()\n", __FUNCTION__);
            return error;
        }

    } break;

    //-----------------------------------------------------------------------------
    // Events
    //
    case IOCTL_IPC_EVENT_OPEN: {

        struct ipc_create_t *ipc_param = (struct ipc_create_t *)_DEVCTL_DATA(msg->i);
        ipc_param->handle = ipc_event_create( entry, ipc_param );
        if(!ipc_param->handle) {
            err_msg(err_trace, "%s(): Error in ipc_event_create()\n", __FUNCTION__);
            error = -EINVAL;
            return error;
        } else {
            nbytes = sizeof(struct ipc_create_t);
        }

    } break;
    case IOCTL_IPC_EVENT_LOCK: {

        struct ipc_lock_t *ipc_param = (struct ipc_lock_t*)_DEVCTL_DATA(msg->i);
        error = ipc_event_lock( entry, ipc_param );
        if(error < 0) {
            err_msg(err_trace, "%s(): Error in ipc_event_lock()\n", __FUNCTION__);
            return error;
        }

    } break;
    case IOCTL_IPC_EVENT_UNLOCK: {

        struct ipc_unlock_t *ipc_param = (struct ipc_unlock_t*)_DEVCTL_DATA(msg->i);
        error = ipc_event_unlock( entry, ipc_param );
        if(error < 0) {
            err_msg(err_trace, "%s(): Error in ipc_event_unlock()\n", __FUNCTION__);
            return error;
        }

    } break;
    case IOCTL_IPC_EVENT_CLOSE: {

        struct ipc_close_t *ipc_param = (struct ipc_close_t *)_DEVCTL_DATA(msg->i);
        error = ipc_event_close( entry, ipc_param );
        if(error < 0) {
            err_msg(err_trace, "%s(): Error in ipc_event_close()\n", __FUNCTION__);
            return error;
        }

    } break;
    case IOCTL_IPC_EVENT_RESET: {

        struct ipc_reset_t *ipc_param = (struct ipc_reset_t *)_DEVCTL_DATA(msg->i);
        error = ipc_event_reset( entry, ipc_param );
        if(error < 0) {
            err_msg(err_trace, "%s(): Error in ipc_event_reset()\n", __FUNCTION__);
            return error;
        }

    } break;

    //-----------------------------------------------------------------------------
    // Shared memory
    //
    case IOCTL_IPC_SHM_OPEN: {

        struct ipc_create_t *ipc_param = (struct ipc_create_t *)_DEVCTL_DATA(msg->i);
        ipc_param->handle = ipc_shm_open( entry, ipc_param );
        if(!ipc_param->handle) {
            err_msg(err_trace, "%s(): Error in ipc_shm_create()\n", __FUNCTION__);
            error = -EINVAL;
            return error;
        } else {
            nbytes = sizeof(struct ipc_create_t);
        }

    } break;
    case IOCTL_IPC_SHM_CLOSE: {

        struct ipc_close_t *ipc_param = (struct ipc_close_t *)_DEVCTL_DATA(msg->i);
        error = ipc_shm_close( entry, ipc_param );
        if(error < 0) {
            err_msg(err_trace, "%s(): Error in ipc_shm_close()\n", __FUNCTION__);
            return error;
        }

    } break;
    default:
        err_msg(err_trace, "%s(): Unknown ioctl command\n", __FUNCTION__);
        error = -EINVAL;
        return error;
	}

	memset(&msg->o, 0, sizeof(msg->o));

	return _RESMGR_PTR(ctp, &msg->o, sizeof(msg->o)+nbytes);
}

//-----------------------------------------------------------------------------

void options (int argc, char **argv)
{
	int     opt;
	int     i;

	optv = 0;

	i = 0;
	while (optind < argc) {
		while ((opt = getopt (argc, argv, "v")) != -1) {
			switch (opt) {
				case 'v':
					optv = 1;
					break;
			}
		}
	}
}

//-----------------------------------------------------------------------------

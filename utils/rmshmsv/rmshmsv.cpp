
//
// Программа для удаления объектов разделяемой памяти SYSTEM V
//

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <errno.h> 
#include <pthread.h> 
#include <sys/shm.h> 
#include <sys/time.h> 
#include <sys/ipc.h> 
#include <sys/types.h> 

//------------------------------------------------------------------------------

#ifndef SHM_R
#define SHM_R 0x100
#endif

#ifndef SHM_W
#define SHM_W 0x80
#endif

#define SVSHM_MODE ( SHM_R | SHM_W | SHM_R>>3 | SHM_R>>6 )

//------------------------------------------------------------------------------

int main (int argc, char *argv[])
{
    key_t key;
    int shmid = -1;

    if(argc != 2) {
        fprintf(stderr, "usage: %s <shmkey>\n", argv[0]);
	return -1;
    }

    key = strtoul(argv[1], NULL, 16);

    if((shmid = shmget(key, 0, SVSHM_MODE)) < 0) {
	perror("shmget");
	return -1;
    }

    //fprintf(stderr, "KEY 0x%x, ID %d\n", key, shmid);

    if( shmctl(shmid, 0, IPC_RMID) < 0 ) {
	perror("shmctl");
	return -1;
    }
	    
    return 0;
}

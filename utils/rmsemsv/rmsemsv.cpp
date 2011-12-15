
//
// Программа для удаления семафоров SYSTEM V из памяти системы
//

//------------------------------------------------------------------------------

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <errno.h> 
#include <pthread.h> 
#include <sys/sem.h> 
#include <sys/time.h> 
#include <sys/ipc.h> 
#include <sys/types.h> 

//------------------------------------------------------------------------------

#ifndef SEM_R
#define SEM_R 0x100
#endif

#ifndef SEM_A
#define SEM_A 0x80
#endif

#define SVSEM_MODE ( SEM_R | SEM_A | SEM_R>>3 | SEM_R>>6 )

//------------------------------------------------------------------------------

int main (int argc, char *argv[])
{
    key_t key;
    int semid = -1;

    if(argc != 2) {
        fprintf(stderr, "usage: %s <semkey>\n", argv[0]);
        return -1;
    }

    key = strtoul(argv[1], NULL, 16);

    if((semid = semget(key, 0, SVSEM_MODE)) < 0) {
        perror("shmget");
        return -1;
    }

    //fprintf(stderr, "KEY 0x%x, ID %d\n", key, semid);

    if( semctl(semid, 0, IPC_RMID) < 0 ) {
        perror("semctl");
        return -1;
    }

    return 0;
}

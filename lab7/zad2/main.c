#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "posixconsts.h"

int main(int argc, char* argv[]){
    if(argc != 3){
        printf("Program need two arguments - num of bakers, and num of suppliers!\n");
        return 0;
    }

    sem_t* furnace_sem = sem_open(FURNACE_SEM, O_RDWR | O_CREAT, S_IRWXU, 1);
    sem_t* table_sem = sem_open(TABLE_SEM, O_RDWR | O_CREAT, S_IRWXU, 1);

    int table_size = MAX_ON_TABLE + 3;
    int furnace_size = MAX_IN_FURNACE + 3;

    int furnace_sh = shm_open(FURNACE_SH, O_RDWR | O_CREAT, S_IRWXU);
    ftruncate(furnace_sh, furnace_size*sizeof(int));
    int* furnace = (int *) mmap(NULL, furnace_size*sizeof(int),  PROT_READ |PROT_WRITE, MAP_SHARED, furnace_sh, 0);
    furnace[TO_TAKE] = -1;
    furnace[TO_ADD] = 0;
    furnace[COUNTER] = 0;

    int table_sh = shm_open(TABLE_SH, O_RDWR | O_CREAT, S_IRWXU);
    ftruncate(table_sh, table_size*sizeof(int));
    int* table = (int *) mmap(NULL, table_size*sizeof(int),  PROT_READ | PROT_WRITE, MAP_SHARED, table_sh, 0);
    table[TO_TAKE] = -1;
    table[TO_ADD] = 0;
    table[COUNTER] = 0;



    char* baker_to_exec[] = {"./baker", argv[1], NULL};
    execvp( baker_to_exec[0], baker_to_exec);

    char* supplier_to_exec[] = {"./supplier", argv[2], NULL};
    execvp(supplier_to_exec[0], supplier_to_exec);

    munmap(table, 8*sizeof(int));
    munmap(furnace, 8*sizeof(int));
    shm_unlink(FURNACE_SH);
    shm_unlink(TABLE_SH);
    return 0;

}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <time.h>
#include <sys/types.h>
#include <sys/sem.h>

#include "consts.h"

int main(int argc, char* argv[]){
    if(argc != 3){
        printf("Program need two arguments - num of bakers, and num of suppliers!\n");
        return 0;
    }

    int furnace_sem = semget((key_t) FURNACE_SEM, 1, 0666 | IPC_CREAT);
    int table_sem = semget((key_t) TABLE_SEM, 1, 0666 | IPC_CREAT);
    union semun arg;
    arg.val = 1;
    semctl(furnace_sem, 0,SETVAL, arg);
    semctl(table_sem, 0,SETVAL, arg);

    int furnace_sh = shmget((key_t) FURNACE_SH, 8*sizeof(int), 0666 | IPC_CREAT);
    int* furnace = (int *) shmat(furnace_sh, NULL, 0);
    furnace[TO_TAKE] = -1;
    furnace[TO_ADD] = 0;
    furnace[COUNTER] = 0;

    int table_sh = shmget((key_t) TABLE_SH, 8*sizeof(int), 0666 | IPC_CREAT);
    int* table = (int *) shmat(table_sh, NULL, 0);
    table[TO_TAKE] = -1;
    table[TO_ADD] = 0;
    table[COUNTER] = 0;

    pid_t pid = fork();
    if(pid == 0) {
        char *baker_to_exec[] = {"./baker", argv[1], NULL};
        execvp(baker_to_exec[0], baker_to_exec);
        return;
    }else{
        char* supplier_to_exec[] = {"./supplier", argv[2], NULL};
        execvp(supplier_to_exec[0], supplier_to_exec);
    }


    return 0;
}
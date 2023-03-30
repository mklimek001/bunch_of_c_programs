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


int sem_to_del, shm_to_del;

void free_memory(){
    semctl(sem_to_del, 1, IPC_RMID);
    shmctl(shm_to_del, IPC_RMID, NULL);
}

int main(int argc, char*argv[]){
    atexit(free_memory);
    int num_of_suppliers = atoi(argv[1]);
    int index = 0;
    int pizza_type, pizzas_on_table;

    int table_sem = semget((key_t) TABLE_SEM, 0, 0);
    sem_to_del = table_sem;
    int table_sh = shmget((key_t) TABLE_SH, 0, 0);
    shm_to_del = table_sh;

    struct sembuf universal_sembuf_start;
    struct sembuf universal_sembuf_ready;
    universal_sembuf_start.sem_op = -1;
    universal_sembuf_start.sem_num = 0;
    universal_sembuf_ready.sem_op = 1;
    universal_sembuf_ready.sem_num = 0;

    int suppliers_sem = semget((key_t) SUPPLIER_SEM, num_of_suppliers , 0666 | IPC_CREAT);
    union semun arg;
    arg.val = 1;
    for(int i = 0; i < num_of_suppliers ; i++){
        semctl(suppliers_sem, i, SETVAL, arg);
    }
    struct sembuf suppliers_sembuf_start;
    struct sembuf suppliers_sembuf_ready;
    suppliers_sembuf_start.sem_op = -1;
    suppliers_sembuf_ready.sem_op = 1;

    while(1){
        suppliers_sembuf_start.sem_num = index;
        suppliers_sembuf_ready.sem_num = index;
        pid_t pid = fork();
        if(pid == 0){
            semop(suppliers_sem, &suppliers_sembuf_start, 1);
            pid_t curr_pid = getpid();

            //get from table
            int* table = (int *) shmat(table_sh, NULL, 0);
            while (table[COUNTER] <= 0 || table[TO_TAKE] == -1) sleep(1);
            semop(table_sem, &universal_sembuf_start, 1);
            pizza_type = table[table[TO_TAKE]];
            table[table[TO_TAKE]] = -1;
            table[TO_TAKE]++;
            table[TO_TAKE] %= MAX_ON_TABLE;
            table[COUNTER]--;
            pizzas_on_table = table[COUNTER];
            if(table[TO_TAKE] == table[TO_ADD]) table[TO_TAKE] = -1;
            semop(table_sem, &universal_sembuf_ready, 1);
            shmdt((const void*)table);

            char date_str[30];
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            strftime(date_str, sizeof(date_str)-1, "%d-%m-%Y %H:%M:%S", t);

            printf("(%d %s) Pobieram pizze: %d. Liczba pizz na stole: %d.\n",curr_pid, date_str, pizza_type, pizzas_on_table);
            sleep(4);

            now = time(NULL);
            t = localtime(&now);
            strftime(date_str, sizeof(date_str)-1, "%d-%m-%Y %H:%M:%S", t);

            printf("(%d %s) Dostarczam pizze: %d.\n",curr_pid, date_str, pizza_type);
            sleep(4);

            semop(suppliers_sem, &suppliers_sembuf_ready, 1);
            return;
        }else{
            index++;
            index %= num_of_suppliers;
        }

    }
    semctl(table_sem, 1, IPC_RMID);
    shmctl(table_sh, IPC_RMID, NULL);

    return 0;
}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <time.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/wait.h>

#include "consts.h"

int sem_to_del, shm_to_del;

void free_memory(){
    semctl(sem_to_del, 1, IPC_RMID);
    shmctl(shm_to_del, IPC_RMID, NULL);
}

int main(int argc, char*argv[]){
    atexit(free_memory);
    int num_of_bakers = atoi(argv[1]);
    int index = 0;
    int pizza_type, pizzas_on_table, pizzas_in_furnace;

    int furnace_sem = semget((key_t) FURNACE_SEM, 0, 0);
    sem_to_del = furnace_sem;
    int table_sem = semget((key_t) TABLE_SEM, 0, 0);
    int furnace_sh = shmget((key_t) FURNACE_SH, 0,0);
    shm_to_del = furnace_sh;
    int table_sh = shmget((key_t) TABLE_SH, 0, 0);
    int* furnace = (int *) shmat(furnace_sh, NULL, 0);
    //int* table = (int *) shmat(table_sh, NULL, 0);

    struct sembuf universal_sembuf_start;
    struct sembuf universal_sembuf_ready;
    universal_sembuf_start.sem_op = -1;
    universal_sembuf_start.sem_num = 0;
    universal_sembuf_ready.sem_op = 1;
    universal_sembuf_ready.sem_num = 0;

    int bakers_sem = semget((key_t) BAKERS_SEM, num_of_bakers, 0666 | IPC_CREAT);
    union semun arg;
    arg.val = 1;
    for(int i = 0; i < num_of_bakers; i++){
        semctl(bakers_sem, i, SETVAL, arg);
    }
    struct sembuf bakers_sembuf_start;
    struct sembuf bakers_sembuf_ready;
    bakers_sembuf_start.sem_op = -1;
    bakers_sembuf_ready.sem_op = 1;

    while(1){
        bakers_sembuf_start.sem_num = index;
        bakers_sembuf_ready.sem_num = index;
        pid_t pid = fork();
        if(pid == 0){
            semop(bakers_sem, &bakers_sembuf_start, 1);
            pid_t curr_pid = getpid();

            srand(time(0));
            pizza_type = rand() % 9;

            char date_str[30];
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            strftime(date_str, sizeof(date_str)-1, "%d-%m-%Y %H:%M:%S", t);

            printf("(%d %s) Przygotowuje pizze: %d.\n",curr_pid, date_str, pizza_type);

            sleep(1);
            now = time(NULL);
            t = localtime(&now);
            strftime(date_str, sizeof(date_str)-1, "%d-%m-%Y %H:%M:%S", t);

            //add to furnace
            while (furnace[COUNTER] == MAX_IN_FURNACE) sleep(1);
            semop(furnace_sem, &universal_sembuf_start, 1);
            furnace[furnace[TO_ADD]] = pizza_type;
            if(furnace[TO_TAKE] == -1) furnace[TO_TAKE] = 0;
            furnace[TO_ADD]++;
            furnace[TO_ADD] %= MAX_IN_FURNACE;
            furnace[COUNTER]++;
            pizzas_in_furnace = furnace[COUNTER];
            semop(furnace_sem, &universal_sembuf_ready, 1);

            printf("(%d %s) Dodalem pizze: %d. Liczba pizz w piecu: %d.\n",curr_pid, date_str, pizza_type, pizzas_in_furnace);
            sleep(4);

            //get from furnace
            while (furnace[COUNTER] == 0) sleep(1);
            semop(furnace_sem, &universal_sembuf_start, 1);
            furnace[furnace[TO_TAKE]] = -1;
            furnace[TO_TAKE]++;
            furnace[TO_TAKE] %= MAX_IN_FURNACE;
            furnace[COUNTER]--;
            pizzas_in_furnace = furnace[COUNTER];
            if(furnace[TO_TAKE] == furnace[TO_ADD]) furnace[TO_TAKE] = -1;
            semop(furnace_sem, &universal_sembuf_ready, 1);

            now = time(NULL);
            t = localtime(&now);
            strftime(date_str, sizeof(date_str)-1, "%d-%m-%Y %H:%M:%S", t);

            //put on table
            int* table = (int *) shmat(table_sh, NULL, 0);
            while (table[COUNTER] == MAX_ON_TABLE) sleep(1);
            semop(table_sem, &universal_sembuf_start, 1);
            table[table[TO_ADD]] = pizza_type;
            if(table[TO_TAKE] == -1) table[TO_TAKE] = 0;
            table[TO_ADD]++;
            table[TO_ADD] %= MAX_ON_TABLE;
            table[COUNTER]++;
            pizzas_on_table = table[COUNTER];
            semop(table_sem, &universal_sembuf_ready, 1);
            shmdt((const void*)table);

            printf("(%d %s) Wyjmuję pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d.\n", curr_pid, date_str, pizza_type, pizzas_in_furnace, pizzas_on_table);

            semop(bakers_sem, &bakers_sembuf_ready, 1);
            return;
        }else{
            index++;
            index %= num_of_bakers;
        }
    }
    semctl(furnace_sem, 1, IPC_RMID);
    shmctl(furnace_sh, IPC_RMID, NULL);
    return 0;
}
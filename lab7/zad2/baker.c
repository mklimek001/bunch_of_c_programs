#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "posixconsts.h"

void disconnect(){
    shm_unlink(FURNACE_SH);
    shm_unlink(TABLE_SH);
}

int main(int argc, char*argv[]){
    atexit(disconnect);
    int num_of_bakers = atoi(argv[1]);
    int index = 0;
    int pizza_type, pizzas_on_table, pizzas_in_furnace;

    sem_t* bakers_sems[num_of_bakers];
    char name[10] = "";
    char bakers_cnt;
    strcpy(name, BAKERS_SEM);
    for(int i = 0; i< num_of_bakers; i++){
        bakers_cnt = i + '0';
        strcat(name, &bakers_cnt);
        bakers_sems[i] = sem_open(name, O_RDWR | O_CREAT, S_IRWXU, 1);
    }

    sem_t* furnace_sem = sem_open(FURNACE_SEM, O_RDWR);
    //sem_t* furnace_sem = sem_open(FURNACE_SEM, O_RDWR | O_CREAT, S_IRWXU, 1);
    sem_t* table_sem = sem_open(TABLE_SEM, O_RDWR);
    //sem_t* table_sem = sem_open(TABLE_SEM, O_RDWR | O_CREAT, S_IRWXU, 1);

    int furnace_sh = shm_open(FURNACE_SH, O_RDWR, S_IRWXU);
    int* furnace = (int *) mmap(NULL, 8*sizeof(int),  PROT_READ |PROT_WRITE, MAP_SHARED, furnace_sh, 0);

    int table_sh = shm_open(TABLE_SH, O_RDWR, S_IRWXU);
    int* table = (int *) mmap(NULL, 8*sizeof(int),  PROT_READ | PROT_WRITE, MAP_SHARED, table_sh, 0);

    while(1){
        pid_t pid = fork();
        if(pid == 0){
            sem_wait(bakers_sems[index]);
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
            sem_wait(furnace_sem);
            furnace[furnace[TO_ADD]] = pizza_type;
            if(furnace[TO_TAKE] == -1) furnace[TO_TAKE] = 0;
            furnace[TO_ADD]++;
            furnace[TO_ADD] %= MAX_IN_FURNACE;
            furnace[COUNTER]++;
            pizzas_in_furnace = furnace[COUNTER];
            sem_post(furnace_sem);

            printf("(%d %s) Dodalem pizze: %d. Liczba pizz w piecu: %d.\n",curr_pid, date_str, pizza_type, pizzas_in_furnace);
            sleep(4);

            printf("Pizza wyjta z pieca\n");
            //get from furnace
            //while (furnace[COUNTER] == 0) sleep(1);
            sem_wait(furnace_sem);
            furnace[furnace[TO_TAKE]] = -1;
            furnace[TO_TAKE] %= MAX_IN_FURNACE;
            furnace[COUNTER]--;
            pizzas_in_furnace = furnace[COUNTER];
            if(furnace[TO_TAKE] == furnace[TO_ADD]) furnace[TO_TAKE] = -1;
            sem_post(furnace_sem);

            now = time(NULL);
            t = localtime(&now);
            strftime(date_str, sizeof(date_str)-1, "%d-%m-%Y %H:%M:%S", t);

            //put on table
            sem_wait(table_sem);

            while (table[COUNTER] == MAX_ON_TABLE) sleep(1);
            sem_wait(table_sem);
            table[table[TO_ADD]] = pizza_type;
            if(table[TO_TAKE] == -1) table[TO_TAKE] = 0;
            table[TO_ADD]++;
            table[TO_ADD] %= MAX_ON_TABLE;
            table[COUNTER]++;
            pizzas_on_table = table[COUNTER];
            sem_post(table_sem);

            printf("(%d %s) Wyjmuję pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d.\n", curr_pid, date_str, pizza_type, pizzas_in_furnace, pizzas_on_table);

            sem_post(bakers_sems[index]);
            return;
        }else{
            index++;
            index %= num_of_bakers;
        }

    }
    munmap(table, 8*sizeof(int));
    munmap(furnace, 8*sizeof(int));
    return 0;
}
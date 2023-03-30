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
    shm_unlink(TABLE_SH);
}


int main(int argc, char*argv[]){
    atexit(disconnect);
    int num_of_suppliers = atoi(argv[1]);
    int index = 0;
    int pizza_type, pizzas_on_table;

    sem_t* supplier_sems[num_of_suppliers];
    char name[13] = "";
    char suppliers_cnt;
    strcpy(name, SUPPLIER_SEM);
    for(int i = 0; i< num_of_suppliers; i++){
        suppliers_cnt = i + '0';
        strcat(name, &suppliers_cnt);
        supplier_sems[i] = sem_open(name, O_RDWR | O_CREAT, S_IRWXU, 1);
    }

    int table_sh = shm_open(TABLE_SH, O_RDWR, S_IRWXU);
    int* table = (int *) mmap(NULL, 8*sizeof(int),  PROT_READ | PROT_WRITE, MAP_SHARED, table_sh, 0);

    sem_t* table_sem = sem_open(TABLE_SEM, O_RDWR);

    while(1){
        pid_t pid = fork();
        if(pid == 0){
            sem_wait(supplier_sems[index]);
            pid_t curr_pid = getpid();

            //get from table
            while (table[COUNTER] == 0) sleep(1);
            sem_wait(table_sem);
            pizza_type = table[table[TO_TAKE]];
            table[table[TO_TAKE]] = -1;
            table[TO_TAKE] %= MAX_ON_TABLE;
            table[COUNTER]--;
            pizzas_on_table = table[COUNTER];
            if(table[TO_TAKE] == table[TO_ADD]) table[TO_TAKE] = -1;
            sem_post(table_sem);

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

            sem_post(supplier_sems[index]);
            return;
        }else{
            index++;
            index %= num_of_suppliers ;
        }

    }
    munmap(table, 8*sizeof(int));
    return 0;
}
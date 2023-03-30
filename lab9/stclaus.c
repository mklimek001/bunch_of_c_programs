#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define ELVES 10
#define REINDEERS 9
#define ELVES_TO_CARE 3
#define COURSES_TO_DO 3


int elves_waiting = 0;
int reindeers_back = 0;
int delivered_gifts = 0;
pthread_t elves_waiting_ids[ELVES_TO_CARE];

pthread_mutex_t reindeer_cntr = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t elves_cntr = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t claus_wake_up = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t claus_cond = PTHREAD_COND_INITIALIZER;


void* st_claus(void* data){
    pthread_mutex_lock(&claus_wake_up);

    while(delivered_gifts < COURSES_TO_DO){
        while (reindeers_back < REINDEERS && elves_waiting < ELVES_TO_CARE) {
            pthread_cond_wait(&claus_cond, &claus_wake_up);
        }
        if(reindeers_back == REINDEERS){
            printf("Mikolaj: Dostarczam zabawki\n");
            sleep(2 + rand()%3);
            reindeers_back = 0;
            delivered_gifts++;
            if(delivered_gifts == COURSES_TO_DO){
                return 0;
            }
        }
        if(elves_waiting == 3){
            printf("Mikolaj: Rozwiozuje problemy elfow %d, %d, %d. \n", (int) elves_waiting_ids[0], (int) elves_waiting_ids[1], (int) elves_waiting_ids[2]);
            sleep(1+ rand()%2);
            elves_waiting = 0;
        }

        pthread_mutex_unlock(&claus_wake_up);
    }

    printf("Mikolaj: Zabawki dostarczono %d razy\n", COURSES_TO_DO);

    return(0);
}

void* reindeer(void* data){
    pthread_t this_id = pthread_self();
    //int self_num = *(int*)data;

    while(delivered_gifts < COURSES_TO_DO){
        int time_to_relax = 5 + rand() % 5;
        sleep(time_to_relax);

        pthread_mutex_lock(&reindeer_cntr);
        pthread_mutex_lock(&claus_wake_up);

        reindeers_back += 1;
        printf("Renifer: czeka %d reniferow na Mikolaja, %d.\n", reindeers_back, (int)this_id);
        if(reindeers_back == REINDEERS){
            printf("Renifer: wybudzam Mikolaja, %d.\n", (int)this_id);
            pthread_cond_broadcast(&claus_cond);

        }
        pthread_mutex_unlock(&reindeer_cntr);
        pthread_mutex_unlock(&claus_wake_up);

        while(reindeers_back > 0){
            sleep(1);
        }

    }

    return((void*) 0);
}

void* elf(void* data){
    pthread_t this_id = pthread_self();
    //int self_num = *(int*)data;

    while(delivered_gifts < COURSES_TO_DO) {

        int time_to_work = 2 + rand() % 3;
        sleep(time_to_work);

        pthread_mutex_lock(&elves_cntr);

        while(elves_waiting == ELVES_TO_CARE){
            sleep(1);
        }

        pthread_mutex_lock(&claus_wake_up);

        elves_waiting_ids[elves_waiting] = this_id;
        elves_waiting += 1;

        printf("Elf: czeka %d elfow na Mikolaja, %d.\n", elves_waiting, (int)this_id);
        if(elves_waiting == ELVES_TO_CARE){
            printf("Elf: wybudzam Mikolaja, %d.\n", (int)this_id);
            pthread_cond_broadcast(&claus_cond);
        }

        pthread_mutex_unlock(&elves_cntr);
        pthread_mutex_unlock(&claus_wake_up);

        while(elves_waiting > 0){
            sleep(1);
        }

    }
    return((void*) 0);
}


int main(){
    srand(time(0));
    pthread_t reindeers_ids[REINDEERS];
    pthread_t elves_ids[ELVES];
    pthread_t st_claus_id;

    pthread_create(&st_claus_id, NULL, &st_claus, NULL);

    for(int i = 0; i < REINDEERS; i++){
        pthread_create(&reindeers_ids[i], NULL, &reindeer, &i);
    }

    for(int i = 0; i < ELVES; i++){
        pthread_create(&elves_ids[i], NULL, &elf, &i);
    }


    pthread_join(st_claus_id, NULL);

}





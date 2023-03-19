#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include <dlfcn.h>

#define BILLION  1000000000.0;

int main(){

    void *handle = dlopen("./libbibl1.so", RTLD_LAZY);
    if(!handle){
        printf("There is a problem with function handler\n");
    }

    void (*tableCreating)();
    tableCreating = (void (*)())dlsym(handle,"createTable");

    void (*checkFile)();
    checkFile = (void (*)())dlsym(handle,"fileStats");

    void (*deleteReservation)();
    deleteReservation = (void (*)())dlsym(handle,"freeMemory");

    struct timespec start, stop;
    double accum;

    int number;
    printf("Give the number of fields to reserve:");
    scanf("%d",&number);


    //reserve memory
    struct tms childtim1;
    if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) {
        perror( "clock gettime" );
        exit( EXIT_FAILURE );
    }

    (*tableCreating)(number);

    if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) {
        perror( "clock gettime" );
        exit( EXIT_FAILURE );
    }

    accum = ( stop.tv_sec - start.tv_sec ) + ( stop.tv_nsec - start.tv_nsec )/BILLION;

    times( &childtim1 );
    printf("Real time to alloc %d fields: %f.\n",number,accum);
    printf( "System time: %ld\n", childtim1.tms_stime);
    printf( "User time: %ld\n", childtim1.tms_utime);



    //free memory
    struct tms childtim2;
    if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) {
        perror( "clock gettime" );
        exit( EXIT_FAILURE );
    }

    //(*deleteReservation)(places);

    if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) {
        perror( "clock gettime" );
        exit( EXIT_FAILURE );
    }

    accum = ( stop.tv_sec - start.tv_sec ) + ( stop.tv_nsec - start.tv_nsec )/ BILLION;

    times( &childtim2 );
    printf("Real time to free %d field: %f.\n",number,accum);
    printf( "System time: %ld\n", childtim2.tms_stime);
    printf( "User time: %ld\n", childtim2.tms_utime);


    char titles[number][100];

    for(int i = 0; i<number;i++){
        char* file_addres = (char *) calloc(100, sizeof(char));

        printf("Give the file number %d: ",i+1);
        scanf("%s",file_addres);

        for(int j = 0; j <= strlen(file_addres); j++){
            titles[i][j] = file_addres[j];
        }

        free(file_addres);

    }


    //files statistics

    if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) {
        perror( "clock gettime" );
        exit( EXIT_FAILURE );
    }


    struct tms childtim3;

    for(int i = 0; i<number;i++){
        char* curr_files = (char *) calloc(100, sizeof(char));

        int name_len = 0;
        for(int j = 0; j < 100; j++){
            curr_files[j] = titles[i][j];
            name_len++;
        }

        printf("%s\n",curr_files);
        (*checkFile)(curr_files, name_len);
        free(curr_files);
    }

    if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) {
        perror( "clock gettime" );
        exit( EXIT_FAILURE );
    }

    accum = ( stop.tv_sec - start.tv_sec ) + (stop.tv_nsec - start.tv_nsec)/BILLION;
    times( &childtim3 );

    printf("Real time to explore %d files: %f.\n",number,accum);
    printf( "System time: %ld\n", childtim3.tms_stime);
    printf( "User time: %ld\n", childtim3.tms_utime);



    return 0;
}

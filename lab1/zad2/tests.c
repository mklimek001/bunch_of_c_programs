#include <stdio.h>
#include "bibl1.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>

#define BILLION  1000000000.0;

int main(){

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

    void *places = createTable(number);

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

    freeMemory(places);

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
        fileStats(curr_files, name_len);
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

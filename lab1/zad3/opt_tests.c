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

    //reserve memory
    struct tms childtim1;
    if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) {
        perror( "clock gettime" );
        exit( EXIT_FAILURE );
    }

    for(int i = 0; i < 100; i++) {
        void *places = createTable((i - i/8)%10);
        freeMemory(places);
    }

    if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) {
        perror( "clock gettime" );
        exit( EXIT_FAILURE );
    }

    accum = ( stop.tv_sec - start.tv_sec ) + ( stop.tv_nsec - start.tv_nsec )/BILLION;

    times( &childtim1 );
    printf("Real time to alloc 100 fields: %f.\n",accum);
    printf( "System time: %ld\n", childtim1.tms_stime);
    printf( "User time: %ld\n", childtim1.tms_utime);


    //files statistics

    if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) {
        perror( "clock gettime" );
        exit( EXIT_FAILURE );
    }
    struct tms childtim3;


    char* curr_files = "./inwokacja.txt";

    for(int i = 0; i < 10; i++){
        fileStats(curr_files, 16);
    }



    if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) {
        perror( "clock gettime" );
        exit( EXIT_FAILURE );
    }

    accum = ( stop.tv_sec - start.tv_sec ) + (stop.tv_nsec - start.tv_nsec)/BILLION;
    times( &childtim3 );

    printf("Real time to explore 10 files: %f.\n",accum);
    printf( "System time: %ld\n", childtim3.tms_stime);
    printf( "User time: %ld\n", childtim3.tms_utime);



    return 0;
}



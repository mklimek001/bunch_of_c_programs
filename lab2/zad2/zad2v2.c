#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>

#define BILLION  1000000000.0;


void browseFile(char *given_file_name, char char_to_count){
    struct timespec start, stop;
    double accum;

    char* file_path = calloc(100,sizeof(char));

    for (int i = 0; i < strlen(given_file_name); i++) {
        file_path[i] = given_file_name[i];
    }

    if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) {
        perror( "clock gettime" );
        exit( EXIT_FAILURE );
    }


    int chars_counter = 0;
    int lines_counter = 0;
    int this_line_chars = 0;
    char curr_char;

    FILE *curr_file = fopen(file_path,"r");

    while((curr_char = fgetc(curr_file)) != EOF) {

        if(curr_char == char_to_count){
            chars_counter++;
            this_line_chars++;
        }

        if(curr_char == '\n'){
            if(this_line_chars > 0){
                lines_counter++;
            }
            this_line_chars = 0;
        }

    }

    fclose(curr_file);

    if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) {
        perror( "clock gettime" );
        exit( EXIT_FAILURE );
    }

    accum = ( stop.tv_sec - start.tv_sec ) + ( stop.tv_nsec - start.tv_nsec )/BILLION;

    printf("\nNumber of %c in %s: %d\n",char_to_count,file_path,chars_counter);
    printf("Number of lines contains %c in %s: %d\n",char_to_count,file_path,lines_counter);
    printf("Content of '%s' browsed in time: %f\n", file_path, accum);
}


int main(){
    char file_name[100];
    char specified_char;
    printf("Give the path to the file and character to count : ");
    scanf("%s %ch",file_name, &specified_char);

    browseFile(file_name,specified_char);

    return 0;
}

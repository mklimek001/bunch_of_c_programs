#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define BILLION  1000000000.0;


void copyFile(char *given_file_name){
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

    int current_file = open(file_path,O_RDONLY);
    int new_file = open("filecpy.txt",O_RDWR | O_CREAT);

    char curr_char;
    int num_of_sgns = 0;
    char* curr_line = calloc(256,sizeof(char));

    while(read(current_file,&curr_char,1)==1) {

        if(curr_char != '\n'){
            curr_line[num_of_sgns] = curr_char;
            num_of_sgns++;
        }

        if(curr_char == '\n'){
            curr_line[num_of_sgns] = '\n';

            int flag = 0;

            if(num_of_sgns > 0){
                for(int i = 0; i <= num_of_sgns; i++){
                    if(curr_line[i] != ' ' && curr_line[i] != '\t' && curr_line[i] != '\n'){
                        flag++;
                        break;
                    }
                }
            }

            if(flag){
                write(new_file, curr_line, num_of_sgns+1);
                memset(curr_line, 0, 256);
            }

            num_of_sgns = 0;

        }

    }

    free(curr_line);

    if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) {
        perror( "clock gettime" );
        exit( EXIT_FAILURE );
    }

    accum = ( stop.tv_sec - start.tv_sec ) + ( stop.tv_nsec - start.tv_nsec )/BILLION;

    printf("Content of '%s' copied in time %f!\n", file_path, accum);
}


int main(){
    char file_name[100];
    printf("Give the path to the file : ");
    scanf("%s",file_name);
    copyFile(file_name);

    return 0;
}

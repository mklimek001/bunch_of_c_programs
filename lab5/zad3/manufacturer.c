#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


int main(int argc, char* argv[])
{
    if(argc != 5){
        printf("Program needs 4 arguments\n");
        return 0;
    }
    printf("FIFO paht: %s\n", argv[1]);
    printf(".txt file path: %s\n", argv[2]);
    int line = atoi(argv[3]);
    printf("Number of line: %d\n", line);
    int num = atoi(argv[4]);
    printf("Number of signs in every transaction: %d\n", num);

    FILE* source_file = fopen(argv[2], "r");
    int flag = 1;
    char buffer = 0;

    if (source_file != NULL){
        while(flag){
            sleep(1000);
            printf("Current data: ");
            FILE* pipe = fopen(argv[1], "w");
            fwrite(argv[3], sizeof(char), 1, pipe);
            for(int i = 0; i < num; i++) {
                buffer = fgetc(source_file);
    		fwrite(&buffer, sizeof(char), 1, pipe);
                if (buffer == '\n') {
                    flag = 0;
                    break;
                }
            }
            printf("\n");
            fclose(pipe);
        }

    }

    FILE* pipe = fopen(argv[1], "w");
    fwrite("aaaaa", sizeof(char), 6, pipe);
    fclose(pipe);

    //wrfd, "abc", 4);
    //fclose(fifo);ite(
    fclose(source_file);

    return 0;
}


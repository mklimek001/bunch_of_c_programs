#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char* argv[])
{
    if(argc != 4){
        printf("Program needs 3 arguments\n");
        return 0;
    }
    printf("FIFO paht: %s\n", argv[1]);
    printf(".txt file path: %s\n", argv[2]);
    int num = atoi(argv[3]);
    printf("Number of signs in every transaction: %d\n", num);
    return 0;
}

#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
    if(argc == 2){
        FILE* handler;
        printf("Only one argument was given.\n");
        if(strcmp("data", argv[1]) == 0){
            printf("Order by data\n");
            handler = popen("mail | tail -n +2", "w");
        } else{
            if(strcmp("nadawca", argv[1]) == 0){
                printf("Order by nadawca\n");
                handler = popen("mail | tail -n +2 | sort -k3d -", "w");
            } else{
                printf("Wrong argument!\n");
            }
        }
        pclose(handler);

    } else{
        if(argc == 4){
            printf("Three arguments were given.\n");
            printf("Address: %s\n", argv[1]);
            printf("Title: %s\n", argv[2]);
            printf("Content: %s\n", argv[3]);
            FILE* handler;

            char cmd[256];
            sprintf(cmd, "mail -s %s %s", argv[2], argv[1]);
            handler = popen(cmd, "w");
            fputs(argv[3], handler);

            pclose(handler);
        } else{
            printf("Wrong number of arguments!\n");
        }
    }
}
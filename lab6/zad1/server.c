#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "init_data.h"

void deleteQueue(){
    msgctl(SERVER_KEY,IPC_RMID, NULL);
}


int main(void) {
    atexit(deleteQueue);
    struct serverPackage curr_server_package;
    struct clientPackage curr_client_package;

    int client_queues[MAX_CLIENTS];
    int clients_counter = 0;
    for(int i = 0; i<MAX_CLIENTS; i++){
        client_queues[i] = -1;
    }

    int server_queue = msgget(SERVER_KEY, 0600 | IPC_CREAT);
    int fd = open("logs.txt", O_WRONLY |  O_APPEND);

    while(1){
        msgrcv(server_queue, &curr_server_package, sizeof(struct serverPackage), -6, 0);
        printf("Sort of received sig: %ld. \n", curr_server_package.type);

        char date_str[30];
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        strftime(date_str, sizeof(date_str)-1, "%d-%m-%Y %H:%M:%S", t);
        char signal_str[4];
        sprintf(signal_str, "%d",  curr_server_package.universal_int);
        char to_file[100] = "";
        strcat(to_file, date_str);
        strcat(to_file, "  ");
        strcat(to_file, signal_str);
        strcat(to_file, "  ");
        strcat(to_file, (const char*) &curr_server_package.content);
        strcat(to_file, "\n\0");
        write(fd,&to_file, strlen((const char*) to_file));

        if(curr_server_package.type == _INIT){
            //open client queue
            printf("INIT option\n");
            int new_client_queue_id = curr_server_package.universal_int;
            client_queues[clients_counter] = msgget(new_client_queue_id, 0);
            curr_client_package.type = _INIT;
            curr_client_package.client_id = clients_counter;
            strcpy(curr_client_package.content, "");
            msgsnd(client_queues[clients_counter], &curr_client_package, sizeof(struct clientPackage),0);
            clients_counter++;

        } else if(curr_server_package.type == _LIST){
            //list all clients
            printf("LIST option\n");
            for(int i = 0; i < MAX_CLIENTS; i++){
                if(client_queues[i] > 0){
                    printf("%d\n", i);
                }
            }
        }else if(curr_server_package.type == _2ALL){
            printf("2ALL option\n");
            printf("Send : %s to all\n", curr_server_package.content);
            curr_client_package.type = _2ALL;
            curr_client_package.client_id = curr_server_package.universal_int;
            strcpy(curr_client_package.content, to_file);
            for(int i = 0; i < MAX_CLIENTS; i++){
                if(client_queues[i] > 0){
                    msgsnd(client_queues[i], &curr_client_package, sizeof(struct clientPackage), 0);
                }
            }

        }else if(curr_server_package.type == _2ONE){
            printf("2ONE option\n");
            printf("Send : %s to %d\n", curr_server_package.content, curr_server_package.universal_int);
            curr_client_package.type = _2ALL;
            int rec_id = curr_server_package.universal_int;
            curr_client_package.client_id = rec_id;
            strcpy(curr_client_package.content, to_file);
            if(client_queues[rec_id] > 0){
                msgsnd(client_queues[rec_id], &curr_client_package, sizeof(struct clientPackage), 0);
            }

        }else if(curr_server_package.type == _STOP){
            printf("STOP option\n");
            client_queues[curr_server_package.universal_int] = -1;
        }

    }
}

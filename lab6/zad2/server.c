#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>

#include "init_data.h"

struct client_queue{
    int is_set;
    mqd_t mqdt_param;
};

mqd_t to_del_queue;

void deleteQueue(){
    mq_close(to_del_queue);
    mq_unlink(SERVER_KEY);
}


int main(void) {
    atexit(deleteQueue);
    struct mq_attr server_atr;
    server_atr.mq_flags = O_CREAT | O_RDONLY;
    server_atr.mq_maxmsg = 100;
    server_atr.mq_msgsize = 50*sizeof(char);
    server_atr.mq_curmsgs = 0;

    struct mq_attr client_atr;
    client_atr.mq_flags = 0;
    client_atr.mq_maxmsg = 100;
    client_atr.mq_msgsize = 100*sizeof(char);
    client_atr.mq_curmsgs = 0;

    unsigned int priority;
    char received_message[100];

    struct client_queue client_queues[MAX_CLIENTS];
    int clients_counter = 0;
    for(int i = 0; i<MAX_CLIENTS; i++){
        client_queues[i].is_set = -1;
    }

    mqd_t server_queue = mq_open(SERVER_KEY, O_CREAT | O_RDONLY , 0644, server_atr);
    printf("%d\n", server_queue);
    to_del_queue = server_queue;
    int fd = open("logs.txt", O_RDWR |  O_APPEND);


    while(1){
        if(mq_receive(server_queue, received_message, 100*sizeof(char), &priority) != -1) {
            printf("Sort of received sig: %d. \n", priority);

            char date_str[30];
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            strftime(date_str, sizeof(date_str) - 1, "%d-%m-%Y %H:%M:%S", t);
            int id = received_message[0] - '0';
            int str_start = 2;
            if (received_message[1] != ' ') {
                id *= 10;
                id += (received_message[1] - '0');
                str_start++;
            }

            char txt[50] = "";
            strcpy(txt, &received_message[str_start]);
            char to_file[100] = "";
            strcat(to_file, date_str);
            strcat(to_file, "  ");
            strcat(to_file, received_message);
            strcat(to_file, "\n\0");
            write(fd, &to_file, strlen((const char *) to_file));

            if (priority == _INIT) {
                //open client queue
                printf("INIT option\n");
                client_queues[clients_counter].is_set = 1;
                client_queues[clients_counter].mqdt_param = mq_open(txt, O_WRONLY);
                char curr_id[100];
                sprintf(curr_id, "%d", clients_counter);
                mq_send(client_queues[clients_counter].mqdt_param, curr_id, 100 * sizeof(char), _INIT);
                clients_counter++;
            }else if(priority == _LIST){
                //list all clients
                printf("LIST option\n");
                for(int i = 0; i < MAX_CLIENTS; i++){
                    if(client_queues[i].is_set > 0){
                        printf("%d\n", i);
                    }
                }
            }else if(priority == _2ALL){
                //send message to all
                printf("2ALL option\n");
                printf("Send : %s to all\n", txt);
                for(int i = 0; i < MAX_CLIENTS; i++){
                    if(client_queues[i].is_set > 0){
                        mq_send(client_queues[i].mqdt_param, to_file, 100 * sizeof(char), _2ALL);
                    }
                }

            }else if(priority == _2ONE){
                //send message to one
                printf("2ONE option\n");
                printf("Send : %s to %d\n", txt, id);
                if(client_queues[id].is_set > 0){
                    mq_send(client_queues[id].mqdt_param, to_file, 100 * sizeof(char), _2ONE);
                }

            }else if(priority == _STOP){
                //stop client work
                printf("STOP option\n");
                mq_close(client_queues[id].mqdt_param);
                client_queues[id].is_set = -1;

            }

        }

    }

}

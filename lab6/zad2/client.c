#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <mqueue.h>
#include <time.h>

#include "init_data.h"

mqd_t queue_to_del;
char client_path_to_del[PATH_LEN];


void endFunc(){
    printf("STOP\n");
    mq_close(queue_to_del);
    mq_unlink(client_path_to_del);
    exit(0);
}

int main(){
    atexit(endFunc);
    signal(SIGINT, endFunc);

    struct mq_attr server_atr;
    server_atr.mq_flags = 0;
    server_atr.mq_maxmsg = 100;
    server_atr.mq_msgsize = 50*sizeof(char);
    server_atr.mq_curmsgs = 0;

    struct mq_attr client_atr;
    client_atr.mq_flags = O_CREAT | O_RDONLY;
    client_atr.mq_maxmsg = 100;
    client_atr.mq_msgsize = 100*sizeof(char);
    client_atr.mq_curmsgs = 0;

    //random path generator
    srand(time(0));
    char own_path[PATH_LEN];
    own_path[0] = '/';
    own_path[PATH_LEN-1] = '\0';
    const char charset[] = "abcdefghijklmnouprstuwxyzABCDEFGHIJKLMNOUPRTSUWXYZ1234567890";
    int charset_len = strlen(charset);
    for(int i = 1; i < PATH_LEN; i++){
        own_path[i] = charset[rand() % (charset_len-1)];
    }
    printf("Random path: %s\n", own_path);

    strcpy(client_path_to_del, own_path);
    mqd_t client_queue = mq_open(own_path, O_CREAT | O_RDONLY , 0644, client_atr);
    //mqd_t client_queue = mq_open(own_path, O_CREAT | O_RDONLY , 0644, client_atr);
    printf("Client %d\n", client_queue);
    queue_to_del = client_queue;

    mqd_t server_queue = mq_open(SERVER_KEY, O_WRONLY);
    printf("Server %d\n", server_queue);
    char init_msg[50] = "0 ";
    strcat(init_msg, own_path);
    if(mq_send(server_queue, (char *)init_msg, 50*sizeof(char), _INIT) == 0){
        printf("Initial message send!\n");
    }

    char received_message[100];
    unsigned int priority;
    while (mq_receive(client_queue, received_message, 100*sizeof(char), &priority) != 0);
    int client_id = atoi(received_message);
    printf("My id is %d\n", client_id);

    char command[5];

    while(1) {
        printf("Enter new command: ");
        scanf("%s", command);
        if (strcmp(command, "STOP") == 0) {
            mq_send(server_queue, "", 50*sizeof(char), _STOP);
            break;
        } else if (strcmp(command, "LIST") == 0) {
            mq_send(server_queue, "", 50*sizeof(char), _LIST);
        } else if (strcmp(command, "2ALL") == 0) {
            char message[100];
            printf("Enter message to send: ");
            scanf("%s", message);
            mq_send(server_queue, message, 50*sizeof(char), _2ALL);
        } else if (strcmp(command, "2ONE") == 0) {
            char message[100];
            int receiver_id;
            printf("Enter message to send: ");
            scanf("%s", message);
            printf("Enter receiver ID: ");
            scanf("%d", &receiver_id);
            mq_send(server_queue, message, 50*sizeof(char), _2ONE);
        } else {
            mq_receive(client_queue, received_message, 100*sizeof(char), &priority);
            if (priority == _2ALL || priority == _2ONE) {
                printf("Received message : %s", received_message);
            }
        }
    }
    return 0;
}

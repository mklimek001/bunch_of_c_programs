#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "init_data.h"

int queue_to_del;
int client_to_del;


void endFunc(){
    printf("STOP\n");
    struct serverPackage to_send;
    to_send.type = _STOP;
    to_send.universal_int = client_to_del;
    strcpy(to_send.content, "");
    int server_queue = msgget(SERVER_KEY, 0);
    msgsnd(server_queue, &to_send, sizeof(struct serverPackage), 0);
    msgctl(queue_to_del, IPC_RMID, NULL);
    exit(0);
}

int main(){
    atexit(endFunc);
    signal(SIGINT, endFunc);

    key_t own_queue_key = ftok(".", 'p');
    int client_queue = msgget(own_queue_key, IPC_CREAT | 0600);
    struct serverPackage new_client_package = {_INIT, own_queue_key, ""};
    queue_to_del = client_queue;

    key_t server_queue_key = SERVER_KEY;
    int server_queue = msgget(server_queue_key, 0);
    msgsnd(server_queue, &new_client_package, sizeof(struct serverPackage), 0);
    printf("Initial message send!\n");

    struct clientPackage received_package;
    msgrcv(client_queue, &received_package, sizeof(struct clientPackage), -6, 0);
    int client_id = received_package.client_id;
    printf("My id is %d\n", client_id);
    client_to_del = client_id;

    struct serverPackage to_send;
    char command[5];

    while(1) {
        printf("Enter new command: ");
        scanf("%s", command);
        if (strcmp(command, "STOP") == 0) {
            to_send.type = _STOP;
            to_send.universal_int = client_id;
            strcpy(to_send.content, "");
            msgsnd(server_queue, &to_send, sizeof(struct serverPackage), 0);
            //msgctl(own_queue_key, IPC_RMID, NULL);
            break;

        } else if (strcmp(command, "LIST") == 0) {
            to_send.type = _LIST;
            to_send.universal_int = client_id;
            strcpy(to_send.content, "");
            msgsnd(server_queue, &to_send, sizeof(struct serverPackage), 0);

        } else if (strcmp(command, "2ALL") == 0) {
            char message[100];
            printf("Enter message to send: ");
            scanf("%s", message);
            printf("To send message : %s\n", message);
            to_send.type = _2ALL;
            to_send.universal_int = client_id;
            strcpy(to_send.content, message);
            msgsnd(server_queue, &to_send, sizeof(struct serverPackage), 0);

        } else if (strcmp(command, "2ONE") == 0) {
            char message[100];
            int receiver_id;
            printf("Enter message to send: ");
            scanf("%s", message);
            printf("Enter receiver ID: ");
            scanf("%d", &receiver_id);
            to_send.type = _2ONE;
            to_send.universal_int = receiver_id;
            strcpy(to_send.content, message);
            msgsnd(server_queue, &to_send, sizeof(struct serverPackage), 0);
        } else {
            msgrcv(client_queue, &received_package, sizeof(struct clientPackage), -6, IPC_NOWAIT);
            if (received_package.type == _2ALL || received_package.type == _2ONE) {
                printf("Received message : %s", received_package.content);
            }
        }
    }

    return 0;
}

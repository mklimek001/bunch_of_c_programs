#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

#define TEST_SIG 9

int sigCntr;

void handlerFunc(int sig_num, siginfo_t *info, void *secret){
    sigCntr++;
}

void handlerEndFunc(int sig_num, siginfo_t *info, void *secret){
    printf("    Number of received signs in sender : %d \n", sigCntr);
}


int main(int argc, char *argv[]) {
    const char* pid = argv[1];
    const char* signumber = argv[2];
    const char* option = argv[3];


    int signumint = 0;
    for(int i = 0; i < strlen(signumber); i++){
        signumint += (int) (signumber[i] - '0') * pow(10, strlen(signumber) - i -1);
    }


    int pidint = 0;
    for(int i = 0; i < strlen(pid); i++){
        pidint += (int) (pid[i] - '0') * pow(10, strlen(pid) - i -1);
    }

    //printf(" PID to send signal : %d\n", pidint);
    //printf("%d\n", signumint);
    //printf("%s\n",option);

    pid_t current_pid = getpid();
    int intpid = (int) current_pid;
    printf("    Sender program with pid : %d\n", intpid);

    sigCntr = 0;

    struct sigaction act, oldact;
    act.sa_sigaction = handlerFunc;
    sigfillset(&act.sa_mask);
    sigdelset(&act.sa_mask,SIGUSR1);
    sigdelset(&act.sa_mask,SIGUSR2);
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, &oldact);

    struct sigaction act2, oldact2;
    act2.sa_sigaction = handlerEndFunc;
    sigfillset(&act2.sa_mask);
    sigdelset(&act2.sa_mask,SIGUSR1);
    sigdelset(&act2.sa_mask,SIGUSR2);
    act2.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR2, &act2, &oldact2);

    if(strcmp(option, "kill") == 0){
        for(int i = 0; i < signumint; i++){
            kill(pidint,SIGUSR1);
        }
        kill(pidint, SIGUSR2);
    }

    if(strcmp(option, "sigqueue") == 0){
        union sigval value;
        for(int i = 0; i < signumint; i++){
            sigqueue(pidint, SIGUSR1, value);
        }
        sigqueue(pidint, SIGUSR2, value);
    }

    if(strcmp(option, "sigrt") == 0){
        sigaction(SIGRTMIN+1, &act, &oldact);
        struct sigaction actrt, oldactrt;
        actrt.sa_sigaction = handlerEndFunc;
        sigfillset(&actrt.sa_mask);
        sigdelset(&actrt.sa_mask,SIGRTMIN+1);
        sigdelset(&actrt.sa_mask,SIGRTMIN+2);
        actrt.sa_flags = SA_SIGINFO;
        sigaction(SIGRTMIN+2 , &actrt, &oldactrt);

        for(int i = 0; i < signumint; i++){
            kill(pidint,SIGRTMIN+1);
        }
        kill(pidint, SIGRTMIN+2);
    }

    while(1){
        sleep(1);
    }

    return 0;
}
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>


#define TEST_SIG 9

int sigCntr, toSend;

void handlerFunc(int sig_num, siginfo_t *info, void *secret){
    sigCntr++;
}

void handlerEndFunc(int sig_num, siginfo_t *info, void *secret){
    printf("Number of received signs in catcher : %d\n", sigCntr);

    int senderpid = info->si_pid;
    for(int i = 0; i < toSend; i++){
        kill(senderpid, SIGUSR1);
    }

    kill(senderpid, SIGUSR2);
}

void handlerEndFuncQueue(int sig_num, siginfo_t *info, void *secret){
    printf("Number of received signs in catcher : %d\n", sigCntr);

    int senderpid = info->si_pid;
    union sigval value;
    for(int i = 0; i < toSend; i++){
        sigqueue(senderpid, SIGUSR1,value);
    }

    sigqueue(senderpid, SIGUSR2, value);
}


void handlerEndFuncRT(int sig_num, siginfo_t *info, void *secret){
    printf("Number of received signs in catcher : %d\n", sigCntr);

    int senderpid = info->si_pid;
    for(int i = 0; i < toSend; i++){
        kill(senderpid, SIGRTMIN+1);
    }

    kill(senderpid, SIGRTMIN+2);
}



int main(int argc, char *argv[]) {
    const char* signumber = argv[1];
    const char* option = argv[2];
    sigCntr = 0;


    int signumint = 0;
    for(int i = 0; i < strlen(signumber); i++){
        signumint += (int) (signumber[i] - '0') * pow(10, strlen(signumber) - i -1);
    }
    toSend = signumint;

    //printf("%d\n", signumint);
    //printf("%s\n", option);

    pid_t current_pid = getpid();
    int intpid = (int) current_pid;
    printf("Catcher program with pid : %d\n", intpid);

    char charpid[10];
    sprintf(charpid, "%d", intpid);

    char signumchar[10];
    sprintf(signumchar, "%d", signumint);

    struct sigaction act, oldact;
    act.sa_sigaction = handlerFunc;
    sigfillset(&act.sa_mask);
    sigdelset(&act.sa_mask,SIGUSR1);
    sigdelset(&act.sa_mask,SIGUSR2);
    act.sa_flags = SA_SIGINFO;


    struct sigaction act2, oldact2;
    sigfillset(&act2.sa_mask);
    sigdelset(&act2.sa_mask,SIGUSR1);
    sigdelset(&act2.sa_mask,SIGUSR2);
    act2.sa_flags = SA_SIGINFO;

    if(strcmp(option, "kill") == 0){
        act2.sa_sigaction = handlerEndFunc;
        sigaction(SIGUSR2, &act2, &oldact2);
        sigaction(SIGUSR1, &act, &oldact);
    }

    if(strcmp(option, "sigqueue") == 0){
        act2.sa_sigaction = handlerEndFuncQueue;
        sigaction(SIGUSR2, &act2, &oldact2);
        sigaction(SIGUSR1, &act, &oldact);
    }


    if(strcmp(option, "sigrt") == 0){
        act2.sa_sigaction = handlerEndFuncRT;
        sigaction(SIGRTMIN+2, &act2, &oldact2);
        sigaction(SIGRTMIN+1, &act, &oldact);
    }


    pid_t p_pid = fork();
    if(p_pid == 0){
        char *args[]={"./sender", (char*) charpid, (char*)signumchar,(char*) option, NULL};
        execvp(args[0],args);
    }

    while(1){
        sleep(1);
    }

    return 0;
}
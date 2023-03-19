#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

#define TEST_SIG 9

int sigCntr, toSend;
int remainingSigs;

void handlerFunc(int sig_num, siginfo_t *info, void *secret){
    sigCntr++;
    int senderpid = info->si_pid;
    if(remainingSigs > 0){
        kill(senderpid, SIGUSR1);
    } else{
        kill(senderpid, SIGUSR2);
    }
}

void handlerEndFunc(int sig_num, siginfo_t *info, void *secret){
    printf("Number of received signs in catcher : %d\n", sigCntr);
}


int main(int argc, char *argv[]) {
    const char* signumber = argv[1];
    sigCntr = 0;

    int signumint = 0;
    for(int i = 0; i < strlen(signumber); i++){
        signumint += (int) (signumber[i] - '0') * pow(10, strlen(signumber) - i -1);
    }
    toSend = signumint;
    remainingSigs = signumint;

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
    sigaction(SIGUSR1, &act, &oldact);


    struct sigaction act2, oldact2;
    sigfillset(&act2.sa_mask);
    sigdelset(&act2.sa_mask,SIGUSR1);
    sigdelset(&act2.sa_mask,SIGUSR2);
    act2.sa_flags = SA_SIGINFO;
    act2.sa_sigaction = handlerEndFunc;
    sigaction(SIGUSR2, &act2, &oldact2);


    pid_t p_pid = fork();
    if(p_pid == 0){
        char *args[]={"./sender", (char*) charpid, (char*)signumchar, NULL};
        execvp(args[0],args);
    }

    while(1){
        sleep(1);
    }

    return 0;
}
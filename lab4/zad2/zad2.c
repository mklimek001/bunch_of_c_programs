#include <stdio.h>
#include <signal.h>
#include <unistd.h>


#define TEST_SIG 10

void handlerFuncSiginfo(int sig_num, siginfo_t *info, void *secret){
    printf("Sigaction handler works!\n");
    printf("Signal number %d\n",sig_num);
    printf("PID of sending process %d\n",info->si_pid);
    printf("UID of sending user %d\n",info->si_uid);
    sleep(2);
    printf("Handler end its work!\n");
}

void handlerFunc(int sig_num){
    printf("Sigaction handler works!\n");
    printf("Signal number %d\n",sig_num);
    printf("Handler end its work!\n");
}


int main() {

    printf("Main function works\n");

    struct sigaction act, oldact;
    //act.sa_sigaction = handlerFuncSiginfo;
    act.sa_handler = handlerFunc;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask,TEST_SIG);
    act.sa_flags = SA_NODEFER;
    sigaction(TEST_SIG, &act, &oldact);

    raise(TEST_SIG);

    /*
    pid_t child_pid;
    child_pid = fork();

    int i = 0;
    if(child_pid == 0){
        while(i < 3){
            raise(9);
            sleep(1);
            i++;
        }

    }
     */

    int j = 0;
    while(j < 3){
        printf("...program works\n");
        fflush(stdout);
        sleep(1);
        j++;
    }

    return 0;
}
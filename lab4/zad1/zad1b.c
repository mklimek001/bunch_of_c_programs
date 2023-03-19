#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define TEST_SIG 6

void handlerFunc(int sig_num){
    printf("Handler works!\n");
    printf("Signal number %d\n",sig_num);
}


int main(int argc, char *argv[]) {
    const char* option = argv[1];

    if(strcmp(option,"ignore") == 0) {
        printf("Ignore option chosen\n");
        signal( TEST_SIG, SIG_IGN);
    }

    if(strcmp(option,"handler") == 0){
        printf("Handler option chosen\n");
        signal (TEST_SIG, handlerFunc);
    }

    if(strcmp(option,"mask") == 0 || strcmp(option,"pending") == 0){
        sigset_t thissig;
        sigset_t oldsigs;
        sigemptyset(&thissig);
        sigaddset(&thissig, TEST_SIG);
        sigprocmask(SIG_SETMASK, &thissig, &oldsigs);
    }

    if(strcmp(option,"mask") == 0){
        printf("Mask option chosen\n");
    }

    if(strcmp(option,"pending") == 0) {
        printf("Pending option chosen\n");
    }

    raise(TEST_SIG);

    if(strcmp(option,"pending") == 0){
        sigset_t pendingsig;

        if(sigpending(&pendingsig) == 0){
            printf("Pending signals : \n");

            for (int signo = 1; signo < _NSIG; signo++) {
                if (sigismember(&pendingsig, signo) == 0)
                    continue;
                else
                    printf("%d\n", signo);
            }
        }

    }

    char *args[]={"./zadexec", (char*)option, NULL};
    execvp(args[0],args);


    int i = 0;
    while(i < 3){
        printf("...program works\n");
        fflush(stdout);
        sleep(1);
        i++;
    }

    return 0;
}
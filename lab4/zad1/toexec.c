#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define TEST_SIG 6


int main(int argc, char *argv[]) {
    const char *option = argv[1];
    printf("Exec program\n");

    if (strcmp(option, "ignore") == 0) {
        printf("Ignore option chosen\n");
    }

    if (strcmp(option, "mask") == 0) {
        printf("Mask option chosen\n");
    }

    if (strcmp(option, "pending") == 0) {
        printf("Pending option chosen\n");

        sigset_t execpendingsig;
        if(sigpending(&execpendingsig) == 0) {
            printf("Pending signals in exec Process : \n");

            for (int signo = 1; signo < _NSIG; signo++) {
                if (sigismember(&execpendingsig, signo) == 0)
                    continue;
                else
                    printf("%d\n", signo);
            }
        }
    }

    raise(TEST_SIG);

    int i = 0;
    while(i < 3){
        printf("...exec program works\n");
        fflush(stdout);
        sleep(1);
        i++;
    }

    return 0;
}
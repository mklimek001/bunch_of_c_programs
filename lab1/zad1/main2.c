#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

int main() {
    //printf("Main function works!\n");

    void *handle = dlopen("./libbibl1.so", RTLD_LAZY);
    if(!handle){
        printf("There is a problem with function handler\n");
    }

    void (*tableCreating)();
    tableCreating = (void (*)())dlsym(handle,"createTable");

    void (*checkFile)();
    checkFile = (void (*)())dlsym(handle,"fileStats");

    void (*deleteReservation)();
    deleteReservation = (void (*)())dlsym(handle,"freeMemory");


    if(dlerror() != NULL){
        printf("There is a error\n");
    }

    (*tableCreating)(10);
    (*checkFile)("./inwokacja.txt", strlen("./inwokacja.txt"));

    dlclose(handle);

}

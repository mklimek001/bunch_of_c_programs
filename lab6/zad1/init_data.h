#define SERVER_KEY 2000
#define SERVER_ID 101
#define MAX_CLIENTS 100

#define _STOP 1
#define _LIST 2
#define _2ALL 3
#define _2ONE 4
#define _INIT 5


struct clientPackage{
    long type;
    int client_id;
    char content[100];
};

struct serverPackage{
    long type;
    int universal_int;
    //universal int is
        // queue key in init
        // receiver_id in 2ALL i 2ONE
    char content[100];
};
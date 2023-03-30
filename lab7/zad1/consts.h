#define MAX_IN_FURNACE 5
#define MAX_ON_TABLE 5

#define BAKERS_SEM 100
#define FURNACE_SEM 200
#define SUPPLIER_SEM 300
#define TABLE_SEM 400
#define TABLE_SH 500
#define FURNACE_SH 600

#define TO_TAKE 5
#define TO_ADD 6
#define COUNTER 7

union semun {
    int val;
    struct semid_ds *buf;
    ushort *array;
};


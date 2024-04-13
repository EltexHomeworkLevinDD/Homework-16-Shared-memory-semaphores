#ifndef SHM_MAP
#define SHM_MAP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <semaphore.h>

#define MAX_NAME_SZ 16
#define MAX_HIST_SZ 4096
#define MAX_MSG_SZ 512
#define MAX_USER_CNT 8

extern const char* shm_name;
extern const char* sem_name;

// Структура разделяемой памяти
struct Shm{
    char names[MAX_USER_CNT][MAX_NAME_SZ];
    char history[MAX_HIST_SZ];
};
typedef struct Shm Shm;

int add_name(char (*names)[MAX_NAME_SZ], const char* name);
void remove_name(char (*names)[MAX_NAME_SZ], const char* name);

#endif
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include "../../include/common/common.h"

int main(){
    // Создаём область разделяемой памяти
    int shmdes = shm_open(shm_name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (shmdes == -1){
        perror("shm_open() creation");
        exit(EXIT_FAILURE);
    }
    // Усекаем размер области разделяемой памяти
    if (ftruncate(shmdes, sizeof(Shm)) == -1){
        perror("ftruncate() ");
        exit(EXIT_FAILURE);
    }

    // Присоединяем область
    void* shmaddr = mmap(NULL, sizeof(Shm), PROT_READ | PROT_WRITE, MAP_SHARED, shmdes, 0);
    if (shmaddr == MAP_FAILED){
        perror("mmap() attach");
        exit(EXIT_FAILURE);
    }

    Shm* server = (Shm*)shmaddr;
    // Создать семафор 
    sem_unlink(sem_name);
    sem_t* access = sem_open(sem_name, O_CREAT, S_IRUSR | S_IWUSR, 1);
    if (access == SEM_FAILED){
        perror("sem_open() creation");
        exit(EXIT_FAILURE);
    }
    // Создать историю и список пользователей
    memset(server->history, '\0', MAX_HIST_SZ);
    memset(server->names, '\0', MAX_USER_CNT*MAX_NAME_SZ);

    char ch = fgetc(stdin);
    (void) ch;

    sem_close(access);

    // Отсоединяем область
    if (munmap(shmaddr, sizeof(Shm)) == -1){
        perror("munmap() detach");
        exit(EXIT_FAILURE);
    }

    // Удаляем область
    if (shm_unlink(shm_name) == -1){
        perror("shm_unlink() remove");
        exit(EXIT_FAILURE);
    }

    return 0;
}
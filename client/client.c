#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <linux/limits.h>
#include "../common/common.h"

int main(){

    // Подключаемся к области разделяемой памяти
    int shmdes = shm_open(keyfile_path, O_RDWR, S_IRUSR | S_IWUSR);
    if (shmdes == -1){
        perror("shm_open() creation");
        exit(EXIT_FAILURE);
    }

    // Присоединяем область
    void* shmaddr = mmap(NULL, sizeof(Shm), PROT_READ | PROT_WRITE, MAP_SHARED, shmdes, 0);
    if (shmaddr == MAP_FAILED){
        perror("mmap() attach");
        exit(EXIT_FAILURE);
    }

    // Приводим указатель и читаем сообщение
    Shm* myshm = (Shm*)shmaddr;
    printf("Readed: '%s'\n", myshm->msg1);

    // Записываем сообшщение
    strncpy(myshm->msg2, "Hello!", MAX_MSG_SIZE);
    myshm->msg2[MAX_MSG_SIZE-1] = '\0';
    myshm->msg1_answer_written = 1;
    printf("Written: '%s'\n", myshm->msg2);

    // Отсоединяем область
    if (munmap(shmaddr, sizeof(Shm)) == -1){
        perror("munmap() detach");
        exit(EXIT_FAILURE);
    }
    return 0;
}
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "../common/common.h"

int main(){

    // Генерируем ключ
    key_t key = ftok(keyfile_path, 0);
    if (key == -1){
        perror("ftok()");
        exit(EXIT_FAILURE);
    }
    // Подключаемся к области разделяемой памяти
    int shmid = shmget(key, sizeof(Shm), 0);
    if (shmid == 0){
        perror("shmget() creation");
        exit(EXIT_FAILURE);
    }
    // Присоединяем область
    void* shmaddr = shmat(shmid, NULL, 0);
    if (shmaddr == (void*)-1){
        perror("shmat() attach");
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
    if (shmdt(shmaddr) == -1){
        perror("shmat() detach");
        exit(EXIT_FAILURE);
    }
    return 0;
}
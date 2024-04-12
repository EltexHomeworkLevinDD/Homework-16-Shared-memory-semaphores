#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "../common/common.h"

int main(){

    // Создаём файл для ключа
    FILE* keyfile;
    keyfile = fopen(keyfile_path, "w");
    if (keyfile == NULL){
        perror("fopen() Creation file");
        exit(EXIT_FAILURE);
    }
    fclose(keyfile);
    // Генерируем ключ
    key_t key = ftok(keyfile_path, 0);
    if (key == -1){
        perror("ftok()");
        exit(EXIT_FAILURE);
    }
    // Создаём область разделяемой памяти
    int shmid = shmget(key, sizeof(Shm), IPC_CREAT | 0600);
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

    // Приводим указатель
    Shm* myshm = (Shm*)shmaddr;
    myshm->msg1_answer_written = 0;

    // Записываем сообщение
    strncpy(myshm->msg1, "Hi!", MAX_MSG_SIZE);
    myshm->msg1[MAX_MSG_SIZE-1] = '\0';
    printf("Written: '%s'\n", myshm->msg1);

    // Ждём ответа
    while(myshm->msg1_answer_written == 0){
        continue;
    }
    // Ответ был записан, читаем
    printf("Readed: '%s'\n", myshm->msg2);

    // Отсоединяем область
    if (shmdt(shmaddr) == -1){
        perror("shmat() detach");
        exit(EXIT_FAILURE);
    }
    // Удаляем область
    if (shmctl(shmid, IPC_RMID, NULL) == -1){
        perror("shmctl() remove");
        exit(EXIT_FAILURE);
    }
    // Удаляем файл ключа
    if (remove(keyfile_path) != 0){
        perror("remove() Removing file");
        exit(EXIT_FAILURE);
    }

    return 0;
}
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

    // Создаём область разделяемой памяти //"/tmp/shm_key_file"   keyfile_path
    int shmdes = shm_open(keyfile_path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
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
    if (munmap(shmaddr, sizeof(Shm)) == -1){
        perror("munmap() detach");
        exit(EXIT_FAILURE);
    }

    // Удаляем область
    if (shm_unlink(keyfile_path) == -1){
        perror("shm_unlink() remove");
        exit(EXIT_FAILURE);
    }

    return 0;
}
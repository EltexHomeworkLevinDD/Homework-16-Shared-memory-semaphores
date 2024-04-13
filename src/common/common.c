#include "../../include/common/common.h"

const char* shm_name = "/myshm";
const char* sem_name = "/mysem";

int add_name(char (*names)[MAX_NAME_SZ], const char* name) {
    // Поиск свободного слота для имени
    int i;
    for (i = 0; i < MAX_USER_CNT; i++) {
        if (strlen(names[i]) == 0) {
            strcpy(names[i], name);
            break;
        }
    }

    // Если список полон, выводим сообщение об ошибке
    if (i == MAX_USER_CNT) {
        printf("List is full\n");
        return -1;
    }

    return 0;
}

void remove_name(char (*names)[MAX_NAME_SZ], const char* name) {
    // Поиск имени в списке и его удаление
    int i;
    for (i = 0; i < MAX_USER_CNT; i++) {
        if (strcmp(names[i], name) == 0) {
            memset(names[i], 0, MAX_NAME_SZ);
            break;
        }
    }
}
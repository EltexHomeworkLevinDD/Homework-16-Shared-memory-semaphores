#include "../../include/client/threads.h"

volatile int uplink_is_dead = 0;

void* uplink(void* uplink_args){

    UplinkArgs* args = (UplinkArgs*)uplink_args;
    char input[MAX_MSG_SZ-3-MAX_NAME_SZ];

    while(1){
        // Ввожу сообщение
        wmove(args->wnd_command->subwnd, 0, 0);
        werase(args->wnd_command->subwnd);

        mvwprintw(args->wnd_command->subwnd, 0, 0, "%s", args->msg_promt);
        wrefresh(args->wnd_command->subwnd);
        if (wgetnstr(args->wnd_command->subwnd, input, MAX_MSG_SZ-3-MAX_NAME_SZ) == ERR){
            perror("wgetnstr() for message in uplink()");
            uplink_is_dead = 1;
            pthread_exit((void*)EXIT_FAILURE);
        }
        // Удаляю символ новой строки 
        input[strcspn(input, "\n")] = '\0';
        
        sem_wait(args->access);

        // Проверить текст на соответствие команде выхода
        if (strncmp(input, args->cmd_exit, strlen(args->cmd_exit)+1) != 0){
            // Не команда выхода
            // Ищу недопустимые символы ('[' или ']')
            char *ptr = strpbrk(input, "[]");
            if (ptr != NULL) {
                //perror("strpbrk(), message contains '[' or ']'");
                continue;
            }
        } else {
            // Команда выхода
            remove_name(args->server->names, args->own_name);
            uplink_is_dead = 1;
            sem_post(args->access);
            // Завершить поток
            return NULL;
        }

        // Сформировать полное собщение
        char* full_message;
        int full_message_size = create_message(input, args->own_name, &full_message);
        if (full_message_size == -1){
            perror("create_message(), uplink()");
            sem_post(args->access);
            uplink_is_dead = 1;
            pthread_exit((void*)EXIT_FAILURE);
        }

        // Сохранить сообщение в историю
        if (save_message_to_history(full_message, args->server->history, MAX_HIST_SZ) != 0){
            perror("save_message_to_history(), uplink()");
            free(full_message);
            sem_post(args->access);
            uplink_is_dead = 1;
            pthread_exit((void*)EXIT_FAILURE);
        }
        free(full_message);

        sem_post(args->access);
    }


    return NULL;
}

void* downlink(void* uplink_args){
    DownlinkArgs* args = (DownlinkArgs*)uplink_args;
    int old_y, old_x;
    while (1){
        if (!uplink_is_dead){
            if (sem_wait(args->access) == -1){
                perror("sem_wait() in uplink()");
                uplink_is_dead = 1;
                pthread_exit((void*)EXIT_FAILURE);
            }

            // Получаем текущие координаты курсора в окне команд
            getyx(args->wnd_command->subwnd, old_y, old_x);

            // Выводим список пользователей
            print_users_list(args->wnd_list, args->server->names);
            // Выводим историю
            print_history(args->wnd_chat, args->server->history);

            // Перемещаем курсор обратно на старые координаты
            wmove(args->wnd_command->subwnd, old_y, old_x);
            wrefresh(args->wnd_command->subwnd);

            sem_post(args->access);

            usleep(100000);
        }
        else {
            return NULL;
        }
    }

    return NULL;
}

/*Сформировать полное сообщение ([Имя] Текст)
Принимает
- text - текст сообщения
- size - размер сообщения
- fullmessage - пустой указатель на сообщение, и
allocate him, освобождайте самостоятельно

Возвращает
- Размер сообщения
- -1 в случае критической ошибки
*/
int create_message(char* text, char* own_name, char** fullmessage){
    int size = strlen(text) + 1;
    int own_name_length = strlen(own_name);
    // Выделяю память под сообщение
    // text_size
    // own_name_length
    // +2 for '[' & ']'
    // +1 for ' ' after ']'
    int fullmessage_size = size+own_name_length+2+1;
    (*fullmessage) = malloc(fullmessage_size);
    if (*fullmessage == NULL){
        return -1;
    }
    int clen = 0;

    // Добавляю открывающую скобку
    (*fullmessage)[clen] = '[';
    clen++;
    // Добавляю имя
    memcpy(*fullmessage + clen, own_name, own_name_length);
    clen += own_name_length;
    // Добавляю закрывающую скобку
    (*fullmessage)[clen++] = ']';
    // Добавляю пробел
    (*fullmessage)[clen++] = ' ';
    // Добавляю текст
    memcpy(*fullmessage + clen, text, size);
    
    return fullmessage_size;
}

int save_message_to_history(char* fullmessage, char* history, int max_history_size) {
    int len = strlen(fullmessage);
    int history_size = strlen(history);
    
    // Если сообщение полностью занимает историю, затираем последний символ и сдвигаем
    if (history_size + len > max_history_size) {
        memmove(history + len, history, max_history_size - len);
        strncpy(history, fullmessage, len);
    } else {
        // Сдвигаем историю вправо на len символов
        memmove(history + len, history, history_size + 1); // +1 для '\0'
        strncpy(history, fullmessage, len);
    }

    history[max_history_size - 1] = '\0';

    return 0;
}

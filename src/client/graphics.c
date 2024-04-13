#include "../../include/client/graphics.h"

void set_text_centered(Window* plank, char* text){
    int len = strlen(text);
    int center = plank->sub_w/2 - len/2;
    mvwprintw(plank->subwnd, 0, center, "%s", text);
}

void create_std_window(Window* wnd){
    wnd->mainwnd = newwin(wnd->main_h, wnd->main_w, wnd->main_y, wnd->main_x);
    wnd->subwnd = derwin(wnd->mainwnd, wnd->sub_h, wnd->sub_w, 1, 1);
    box(wnd->mainwnd, 0,0);
}

void create_std_plank(Window* wnd, char* text){
    create_std_window(wnd);
    set_text_centered(wnd, text);
}

void refresh_std_window_r(Window* wnd){
    wrefresh(wnd->mainwnd);
    wrefresh(wnd->subwnd);
}

void delete_std_window_r(Window* wnd){
    wclear(wnd->mainwnd);
    wclear(wnd->subwnd);

    refresh_std_window_r(wnd);

    delwin(wnd->subwnd);
    delwin(wnd->mainwnd);
}

/*Ввести имя (до max_name_size-1 символов)
Перед вызовом требуется очистить command_current_content.
Сохраняет введённое содержимое в command_current_content
Очищате name
- posx - начальная позиция ввода (если указывает не на конец command_current_content,
в ввод зачтётся текущее содержимое после pos)

Возвращает:
- 0 - всё ок
- -1 - запрошена команда cmd_exit
- -2 - если строка содержит недопустимые символы ('[' или ']')
- -3 - критическая ошибка*/
int request_name(Window* wnd, char* promt_text, char** name, int max_name_size, char* cmd_exit){
    // Очищаем окно
    wclear(wnd->subwnd);
    // Промт
    memset(*name, '\0', max_name_size);
    wmove(wnd->subwnd, 0, 0);
    mvwprintw(wnd->subwnd, 0, 0, "%s ", promt_text);
    //wmove(wnd->subwnd, 0, promt_text_len+1);
    // Ввожу имя
    int res = wgetnstr(wnd->subwnd, *name, max_name_size);
    // Если произошла критическая ошибка
    if (res == ERR ){
        return -3;
    // Если размер окна был изменён
    } else if (res == KEY_RESIZE){
        memset(*name, '\0', MAX_NAME_SZ);
        return -3;
    }
    // Проверяю на соответствие команде cmd_exit
    if (strncmp(*name, cmd_exit, strlen(cmd_exit)+1) != 0){
        // Не совпало, это имя
        // Ищу недопустимые символы ('[' или ']')
        char *ptr = strpbrk(*name, "[]");
        if (ptr == NULL) {
            // Нет недопустимых символов
            return 0;
        }
        else {
            // Есть недопустимые символы
            return -2;
        }
    }
    // Совпало это команда выхода
    else {
        return -1;
    }
}

void print_users_list(Window* target, char (*names)[MAX_NAME_SZ]){
    // Очищаем содержимое подокна
    werase(target->subwnd);

    // Отрисовываем список пользователей
    for (int i = 0; i < MAX_USER_CNT; i++){
        //mvwprintw(target->subwnd, i, 0, "%s", names[i]);
        if (names[i][0] != 0){
            wprintw(target->subwnd, "%s\n", names[i]);
        }
    }

    // Обновляем подокно
    wrefresh(target->subwnd);
}

void print_history(Window* target, char* history){
    // Очищаем содержимое подокна
    werase(target->subwnd);

    char* str_begin = history;
    char* str_end = str_begin;
    // Длина подстроки
    int str_len = -1;
    // Размер подстроки в строках окна
    int str_lines_size = 0;
    // Позиция подстроки в строках окна
    int str_line_pos = target->sub_h-1;
    // Заполняем чат с низа, пока он не заполнился или не кончилась история
    while (str_end != NULL && str_line_pos > 0){
        // Получаем указатель на строку и её длину, указатель на конец строки
        str_len = find_message_str(str_begin, &str_end);
        // Сколько строк занимает сообщение в окне
        str_lines_size = str_len % target->sub_w == 0 ? str_len/target->sub_w : str_len/target->sub_w + 1;
        // На какой позиции оно
        str_line_pos -= str_lines_size;
        // Печатаем строку по указателю и размеру
        mvwaddnstr(target->subwnd, str_line_pos, 0, str_begin, str_len);
        //mvprintw(target->subwnd, str_line_pos, 0, str_begin, str_len);
        // Получаем указатель на начало следующего сообщения
        str_begin = str_end+1;
    }

    wrefresh(target->subwnd);
}

/*
Передайте указатель на начало строки str_begin 
чтобы получить указатель на конец строки end (end = NULL, если достигнут конец строки)
и return длину строки
*/
int find_message_str(char* str_begin, char** end){
    int index = 0;

    while (1){
        index++;
        if (str_begin[index] == '['){
            index--;
            *end = &(str_begin[index]);
            break;
        } else if (str_begin[index] == '\0') {
            index--;
            *end = NULL;
            break;
        }
    }

    return index+1;
}

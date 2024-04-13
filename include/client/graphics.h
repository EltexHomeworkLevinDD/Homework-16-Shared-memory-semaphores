#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <malloc.h>
#include "../../include/common/common.h"

typedef struct {
    WINDOW* mainwnd;
    WINDOW* subwnd;
    int main_x;
    int main_y;
    int main_h;
    int main_w;
    int sub_h;
    int sub_w;
} Window;

void set_text_centered(Window* plank, char* text);

void create_std_window(Window* wnd);
void create_std_plank(Window* wnd, char* text);
void delete_std_window_r(Window* wnd);
void refresh_std_window_r(Window* wnd);

int request_name(Window* wnd, char* promt_text, char** name, int max_name_size, char* cmd_exit);
void print_users_list(Window* target, char (*names)[MAX_NAME_SZ]);

int find_message_str(char* str_begin, char** end);
void print_history(Window* target, char* history);

#endif//GRAPHICS_H
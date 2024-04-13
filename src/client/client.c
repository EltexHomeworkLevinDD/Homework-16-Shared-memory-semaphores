#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/mman.h>
#include <string.h>
#include "../../include/client/graphics.h"
#include "../../include/client/threads.h"

#define PLANK_HEIGHT 3
// Должен быть больше или равен MAX_OWN_NAME_SIZE+2
#define USERS_LIST_WIDTH (MAX_NAME_SZ+2+5)
#define COMMAND_WINDOW_HEIGHT 5

WINDOW* wnd_terminal;
int wnd_terminal_h = 0;
int wnd_terminal_w = 0;

Window wnd_plank_chat;
Window wnd_chat;

Window wnd_list;
Window wnd_plank_list;

Window wnd_command;
Window wnd_plank_command;

void calc_positions();
void create_interface();
void destroy_interface();

int main(){

    // Подключаемся к области разделяемой памяти
    int shmdes = shm_open(shm_name, O_RDWR, S_IRUSR | S_IWUSR);
    if (shmdes == -1){
        perror("shm_open() connection");
        exit(EXIT_FAILURE);
    }

    // Присоединяем область
    void* shmaddr = mmap(NULL, sizeof(Shm), PROT_READ | PROT_WRITE, MAP_SHARED, shmdes, 0);
    if (shmaddr == MAP_FAILED){
        perror("mmap() attach");
        exit(EXIT_FAILURE);
    }

    // Приводим указатель
    Shm* server = (Shm*)shmaddr;
    // Открываем семафор
    sem_t* access = sem_open(sem_name, 0, S_IRUSR | S_IWUSR, 1);
    if (access == SEM_FAILED){
        perror("sem_open() opening");
        exit(EXIT_FAILURE);
    }

    char* cmd_exit = "/exit";
    char* msg_promt = ">: ";

    // Запустить графику
    initscr();
    cbreak();
    start_color();
    keypad(stdscr, false);
    refresh();

    create_interface();

    // Формирую строку приглашения
    char promt_text[100];
    snprintf(promt_text, sizeof(promt_text), "Exit command: '/exit', your text must exclude '[' and ']'\nEnter your name (Max %d symbols) :> ", MAX_NAME_SZ-1);
    char own_name[MAX_NAME_SZ];

    // Ввожу имя
    int result; 
    char* own_name_ptr = own_name;
    while ((result = request_name(&wnd_command, promt_text, &own_name_ptr, MAX_NAME_SZ, (char*)msg_promt)) != 0){
        // Запрошен выход
        if (result == -1){
            destroy_interface();
            endwin();
            return 0;
        // Строка содержит недопустымые символы, повторить ввод
        } else {
            continue;
        }
    }

    // Записываем своё имя в список
    if (sem_wait(access) == -1){
        perror("sem_wait() in main()");
        exit(EXIT_FAILURE);
    }
    add_name(server->names, (const char*)own_name);
    sem_post(access);

    // Очистить окно
    werase(wnd_command.subwnd); // Очистка указанного окна
    wrefresh(wnd_command.subwnd); // Обновление указанного окна

    // Запускаю потоки чтения и записи


    // Создать поток Uplink
    pthread_t uplink_tid;
    int* uplink_status;
    UplinkArgs uplink_args = {
        .cmd_exit = cmd_exit,
        .msg_promt = msg_promt,
        .own_name = own_name,
        .wnd_command = &wnd_command,
        .server = server,
        .access = access
    };

    if (pthread_create(&uplink_tid, NULL, uplink, (void*)&uplink_args) != 0){
        perror("Creating uplink() thread");
        destroy_interface();
        endwin();
        exit(EXIT_FAILURE);
    }

    // Создать поток Downlink
    pthread_t downlink_tid;
    int* downlink_status;
    DownlinkArgs downlink_args = {
        .wnd_chat = &wnd_chat,
        .wnd_list = &wnd_list,
        .wnd_command = &wnd_command,
        .server = server,
        .access = access
    };
    
    if (pthread_create(&downlink_tid, NULL, downlink, (void*)&downlink_args) != 0){
        perror("Creating downlink() thread");
        destroy_interface();
        endwin();
        exit(EXIT_FAILURE);
    }

    // Ждать завершение потока Uplink
    pthread_join(uplink_tid, (void**)&uplink_status);
    if (uplink_status != NULL){
        destroy_interface();
        endwin();
        perror("Uplink() has broken");
        exit(EXIT_FAILURE);
    }

    // Ждать завершение потока Downlink
    pthread_join(downlink_tid, (void**)&downlink_status);
    if (downlink_status != NULL){
        destroy_interface();
        endwin();
        perror("downlink() has broken");
        exit(EXIT_FAILURE);
    }

    // Уничтожаем интерфейс и прекращаем работу с графикой
    destroy_interface();
    endwin();

    sem_close(access);
    //sem_unlink(sem_name);

    // Отсоединяем область
    if (munmap(shmaddr, sizeof(Shm)) == -1){
        perror("munmap() detach");
        exit(EXIT_FAILURE);
    }
    

    return 0;
}

void create_interface(){
    //  Создать окно терминала
    wnd_terminal = newwin(0, 0, 0, 0);
    getmaxyx(wnd_terminal, wnd_terminal_h, wnd_terminal_w);
    calc_positions();

    // Создать командную плашку и окно
    create_std_plank(&wnd_plank_command, "Command window");
    wrefresh(wnd_plank_command.mainwnd);
    create_std_window(&wnd_command);
    wrefresh(wnd_command.mainwnd);
    // Создать чат плашку и окно
    create_std_plank(&wnd_plank_chat, "Chat window");
    wrefresh(wnd_plank_chat.mainwnd);
    create_std_window(&wnd_chat);
    wrefresh(wnd_chat.mainwnd);
    // Создать список плашку и окно
    create_std_plank(&wnd_plank_list, "Users list");
    wrefresh(wnd_plank_list.mainwnd);
    create_std_window(&wnd_list);
    wrefresh(wnd_list.mainwnd);
}

void calc_positions(){
    // Command Window
    wnd_command.main_h = COMMAND_WINDOW_HEIGHT;
    wnd_command.main_w = wnd_terminal_w;
    wnd_command.main_y = wnd_terminal_h-COMMAND_WINDOW_HEIGHT;
    wnd_command.main_x = 0;
    wnd_command.sub_h = wnd_command.main_h - 2;
    wnd_command.sub_w = wnd_command.main_w - 2;
    // Command Plank
    wnd_plank_command.main_h = PLANK_HEIGHT;
    wnd_plank_command.main_w = wnd_terminal_w;
    wnd_plank_command.main_y = wnd_command.main_y - PLANK_HEIGHT;
    wnd_plank_command.main_x = 0;
    wnd_plank_command.sub_h = wnd_plank_command.main_h - 2;
    wnd_plank_command.sub_w = wnd_plank_command.main_w - 2;
    // Users list Plank
    wnd_plank_list.main_h = PLANK_HEIGHT;
    wnd_plank_list.main_w = USERS_LIST_WIDTH;
    wnd_plank_list.main_y = 0;
    wnd_plank_list.main_x = wnd_terminal_w-USERS_LIST_WIDTH;
    wnd_plank_list.sub_h = wnd_plank_list.main_h - 2;
    wnd_plank_list.sub_w = wnd_plank_list.main_w - 2;
    // Users list Window
    wnd_list.main_h = wnd_terminal_h - COMMAND_WINDOW_HEIGHT-PLANK_HEIGHT*2;
    wnd_list.main_w = USERS_LIST_WIDTH;
    wnd_list.main_y = PLANK_HEIGHT;
    wnd_list.main_x = wnd_terminal_w-USERS_LIST_WIDTH;
    wnd_list.sub_h = wnd_list.main_h - 2;
    wnd_list.sub_w = wnd_list.main_w - 2;
    // Chat Plank
    wnd_plank_chat.main_h = PLANK_HEIGHT;
    wnd_plank_chat.main_w = wnd_terminal_w - wnd_plank_list.main_w;
    wnd_plank_chat.main_y = 0;
    wnd_plank_chat.main_x = 0;
    wnd_plank_chat.sub_h = wnd_plank_chat.main_h - 2;
    wnd_plank_chat.sub_w = wnd_plank_chat.main_w - 2;
    // Chat Window
    wnd_chat.main_h = wnd_terminal_h - COMMAND_WINDOW_HEIGHT-PLANK_HEIGHT*2;
    wnd_chat.main_w = wnd_terminal_w - wnd_plank_list.main_w;
    wnd_chat.main_y = PLANK_HEIGHT;
    wnd_chat.main_x = 0;
    wnd_chat.sub_h = wnd_chat.main_h - 2;
    wnd_chat.sub_w = wnd_chat.main_w - 2;
}

void destroy_interface(){
    delete_std_window_r(&wnd_plank_command);
    delete_std_window_r(&wnd_command);
    delete_std_window_r(&wnd_plank_chat);
    delete_std_window_r(&wnd_chat);
    delete_std_window_r(&wnd_plank_list);
    delete_std_window_r(&wnd_list);
}
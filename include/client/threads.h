#ifndef THREADS_H
#define THREADS_H

#include "../../include/client/graphics.h"
#include <pthread.h>
#include <unistd.h>

struct UplinkArgs {
    Window* wnd_command;
    char* cmd_exit;
    char* msg_promt;
    char* own_name;
    sem_t* access;
    Shm* server;
};

struct DownlinkArgs {
    Window* wnd_command;
    Window* wnd_chat;
    Window* wnd_list;
    sem_t* access;
    Shm* server;
};

typedef struct UplinkArgs UplinkArgs;
typedef struct DownlinkArgs DownlinkArgs;

extern volatile int uplink_is_dead;

void* uplink(void* uplink_args);
void* downlink(void* uplink_args);

int save_message_to_history(char* fullmessage, char* history, int max_history_size);
int create_message(char* text, char* own_name, char** fullmessage);

#endif//THREADS_H
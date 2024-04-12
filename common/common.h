#ifndef COMMON_H
#define COMMON_H

#define MAX_MSG_SIZE 128

extern const char* keyfile_path;

typedef struct {
    char msg1[MAX_MSG_SIZE];
    char msg2[MAX_MSG_SIZE];
    int msg1_answer_written;
} Shm;

#endif//COMMON_H
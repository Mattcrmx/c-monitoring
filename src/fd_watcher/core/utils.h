#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

enum MODE { LOGGER, STATISTICAL };

typedef struct Arguments {
    char *name;
    int pid;
    int time;
    int interval;
    int stats;
    enum MODE mode;
} Arguments;

#endif // UTILS_H

int safe_convert_to_int(char *pid);
int process_exists(int pid);
int get_pid_by_name(char name[]);
void *safe_malloc(size_t size);
char *get_name_from_pid(int pid);
Arguments *new_args(int interval, int time, char *name, int pid, int stats);
Arguments *new_empty_args(void);

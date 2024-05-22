#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

enum MODE { LOGGER, STATISTICAL };

typedef struct Arguments {
    char *name;
    int pid;
    int time;
    float interval;
    int stats;
    enum MODE mode;
} Arguments;

int safe_convert_to_int(char *pid);
int process_exists(int pid);
int get_pid_by_name(char name[]);
void *safe_malloc(size_t size);
char *get_name_from_pid(int pid);

#endif // UTILS_H

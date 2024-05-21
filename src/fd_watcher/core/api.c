
#include <argp.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "utils.h"
#include "monitor.h"

Arguments *new_args(int interval, int time, char *name, int pid, int stats) {
    Arguments *args = (Arguments *)safe_malloc(sizeof(Arguments));
    args->interval = interval;
    args->time = time;
    args->name = name;
    args->pid = pid;
    args->stats = stats;

    return args;
}

Arguments *new_empty_args(void) {
    Arguments *args = (Arguments *)safe_malloc(sizeof(Arguments));
    args->interval = 1;
    args->time = 60;
    args->name = NULL;
    args->pid = -1;
    args->stats = 0;
    return args;
}

DescriptorsArray *new_desc_array(void) {
    DescriptorsArray *desc =
        (DescriptorsArray *)safe_malloc(sizeof(DescriptorsArray));
    desc->descriptors = NULL;
    desc->timestamps = NULL;
    desc->length = 0;
    return desc;
}

int literal_watch(int interval, int time, char *name, int pid, int stats) {
    int ret_code = 1;
    Arguments args;
    args = *new_args(interval, time, name, pid, stats);
    ret_code = watch(args);
    return ret_code;
}

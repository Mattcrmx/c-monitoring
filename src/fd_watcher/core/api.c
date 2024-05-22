
#include <argp.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "utils.h"
#include "monitor.h"
#include "api.h"

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

int literal_watch(float interval, int time, char *name, int pid, int stats) {
    int ret_code = 1;
    Arguments args;
    args = *new_args(interval, time, name, pid, stats);
    ret_code = watch(args);
    return ret_code;
}

/**
 * @brief Wrapper for statistics generation by passing struct directly.
 *
 * @param pid The process ID.
 * @param interval Sampling interval in seconds.
 * @param time_limit Maximum duration in seconds.
 * @return DescriptorsArray with statistics, or invalid array if process
 * doesn't exist or on error.
 *
 */
DescriptorsArray generate_fd_stats_by_value(int pid, float interval,
                                            int time_limit) {

    DescriptorsArray *desc_array =
        (DescriptorsArray *)safe_malloc(sizeof(DescriptorsArray));
    desc_array = generate_fd_stats(pid, interval, time_limit);

    if (desc_array == NULL) {
        // Handle error by sending an invalid array and checking inside the
        // binding
        DescriptorsArray error_array;
        error_array.length = -1;
        error_array.descriptors = NULL;
        error_array.timestamps = NULL;

        return error_array;
    } else {
        return *desc_array;
    }
}

#ifndef MONITOR_H
#define MONITOR_H

#include "utils.h"

typedef struct DescriptorsArray {
    int *descriptors;
    int *timestamps;
    int length;
} DescriptorsArray;

int watch(Arguments arguments);
DescriptorsArray *generate_fd_stats(int pid, int interval, int time_limit);
DescriptorsArray generate_fd_stats_by_value(int pid, int interval,
                                            int time_limit);

#endif // MONITOR_H

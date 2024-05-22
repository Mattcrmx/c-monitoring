#ifndef MONITOR_H
#define MONITOR_H

#include "utils.h"

typedef struct DescriptorsArray {
    int *descriptors;
    long *timestamps;
    int length;
} DescriptorsArray;

int watch(Arguments arguments);
DescriptorsArray *generate_fd_stats(int pid, float interval, int time_limit);

#endif // MONITOR_H

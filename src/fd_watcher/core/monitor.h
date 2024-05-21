#ifndef MONITOR_H
#define MONITOR_H

#include "utils.h"

typedef struct DescriptorsArray {
    int *descriptors;
    int *timestamps;
    int length;
} DescriptorsArray;

int watch(Arguments arguments);
DescriptorsArray *new_desc_array(void);

#endif // MONITOR_H

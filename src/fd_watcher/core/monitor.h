#ifndef MONITOR_H
#define MONITOR_H

#include "utils.h"

typedef struct DescriptorsArray {
    int *descriptors;
    int *timestamps;
    int length;
} DescriptorsArray;

#endif // MONITOR_H

int watch(Arguments arguments);
DescriptorsArray *new_desc_array(void);

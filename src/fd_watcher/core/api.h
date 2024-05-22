#ifndef API_H
#define API_H

#include "utils.h"
#include "monitor.h"

Arguments *new_args(int interval, int time, char *name, int pid, int stats);
Arguments *new_empty_args(void);
DescriptorsArray *new_desc_array(void);
DescriptorsArray generate_fd_stats_by_value(int pid, int interval,
                                            int time_limit);
int literal_watch(int interval, int time, char *name, int pid, int stats);

#endif // API_H

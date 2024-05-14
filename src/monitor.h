#ifndef MONITOR_H
#define MONITOR_H

struct DescriptorsArray {
    int *descriptors;
    int *timestamps;
    int length;
};

#endif // MONITOR_H

int watch(int pid, int interval, int time_limit);
int count_descriptors_by_pid(int pid);
struct DescriptorsArray *generate_fd_stats(int pid, int interval,
                                           int time_limit);
int write_stats_to_csv(struct DescriptorsArray *desc_array, char *process_name);
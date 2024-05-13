int watch(int pid, int interval, int time_limit);
int count_descriptors_by_pid(int pid);
int *watch_with_stats(int pid, int interval, int time_limit);
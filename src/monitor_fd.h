void *safe_malloc(size_t size);
int count_descriptors_by_name (char name[]);
int count_descriptors_by_pid (int pid);
int get_pid_by_name (char name[]);
int process_exists(int pid);
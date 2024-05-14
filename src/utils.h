int safe_convert_to_int(char *pid);
int is_number(char *nb);
int process_exists(int pid);
int get_pid_by_name(char name[]);
void *safe_malloc(size_t size);
char *trim(char *str);

#ifndef UTILS_H
#define UTILS_H

struct arguments {
    char *name;
    int pid;
    int time;
    int interval;
    int stats;
};

#endif // UTILS_H

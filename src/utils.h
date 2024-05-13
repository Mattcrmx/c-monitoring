#include <argp.h>
int safe_convert_to_int(char *pid);
int check_args(int args, int required);
int is_number(char *nb);
int process_exists(int pid);
int get_pid_by_name (char name[]);
void *safe_malloc(size_t size);
char *trim(char *str);

error_t parse_opt(int key, char *arg, struct argp_state *state);

struct arguments
{
    char *name;
    int pid;
    int time;
    int interval;
};
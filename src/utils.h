#include <argp.h>
int safe_convert_to_int(char *pid);
void *safe_malloc(size_t size);
char *trim(char *str);
int is_number(char *nb);
int process_exists(int pid);
error_t parse_opt(int key, char *arg, struct argp_state *state);
struct arguments
{
    enum
    {
        WATCH_BY_NAME,
        WATCH_BY_PID
    } mode;
    char *name;
    int pid;
    int time;
    int interval;
};
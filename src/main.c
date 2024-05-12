#include <stdio.h>
#include <argp.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "monitor_fd.h"
#include "utils.h"

const char *argp_program_version = "fd-watcher 0.1";
static char doc[] = "File descriptor watcher";
static char args_doc[] = "[FILENAME]...";
static struct argp_option options[] = {
    {"time", 't', "TIME", OPTION_ARG_OPTIONAL, "How much time should the watchdog run.", 0},
    {"name", 'n', "NAME", OPTION_ARG_OPTIONAL, "The name of the process to monitor.", 0},
    {"pid", 'p', "PID", OPTION_ARG_OPTIONAL, "The pid of the process to monitor.", 0},
    {"interval", 'i', "INTERVAL", OPTION_ARG_OPTIONAL, "The interval between watch, in seconds.", 0},
    {0} // needs to be terminated by a zero
};
static struct argp argp = {options, parse_opt, args_doc, doc, 0, 0, 0};

int main(int argc, char *argv[])
{

    int parsing_result;
    int pid;

    struct arguments arguments;
    arguments.mode = WATCH_BY_NAME;
    arguments.name = (char *)""; // ugly casting to a pointer to avoid the warning
    arguments.pid = -1;
    arguments.time = 60;
    arguments.interval = 1;

    parsing_result = argp_parse(&argp, argc, argv, 0, 0, &arguments);

    if (parsing_result == 1)
    {
        fprintf(stderr, "Bad usage\n");
        return 1;
    }

    switch (arguments.mode)
    {
    case WATCH_BY_NAME:
        pid = get_pid_by_name(arguments.name);
        break;
    case WATCH_BY_PID:
        pid = arguments.pid;
        break;
    }

    watch(pid, arguments.interval, arguments.time);

    return 0;
};

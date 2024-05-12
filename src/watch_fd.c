#include <stdio.h>
#include <argp.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "monitor_fd.h"

int watch(int pid, int interval, int time_limit);
int safe_convert_to_int(char *pid);
static error_t parse_opt(int key, char *arg, struct argp_state *state);

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
static struct argp argp = {options, parse_opt, args_doc, doc, 0, 0, 0};


/**
 * @brief Converts a string representation of an integer to an integer value safely.
 *
 * This function converts the input string `pid` to an integer value. It checks for
 * invalid input, overflow, and underflow.
 *
 * @param pid Pointer to the string to convert.
 * @return Returns the integer value if successful, otherwise returns -1 and prints an error message to stderr.
 */
int safe_convert_to_int(char *pid)
{
    // helper function to be sure that the provided pid is well formatted.
    // courtesy of https://stackoverflow.com/questions/8871711/atoi-how-to-identify-the-difference-between-zero-and-error
    long lnum;
    int num;
    char *end;

    errno = 0;

    lnum = strtol(pid, &end, 10);

    if (end == pid)
    {
        // if no characters were converted these pointers are equal
        fprintf(stderr, "ERROR: can't convert string to number\n");
        num = -1;
    }
    else if ((lnum == LONG_MAX || lnum == LONG_MIN) && errno == ERANGE)
    {
        // If sizeof(int) == sizeof(long), we have to explicitly check for overflows
        fprintf(stderr, "ERROR: number out of range for LONG\n");
        num = -1;
    }
    else if ((lnum > INT_MAX) || (lnum < INT_MIN))
    {
        // Because strtol produces a long, check for overflow
        fprintf(stderr, "ERROR: number out of range for INT\n");
        num = -1;
    }
    else
    {
        num = (int)lnum;
    }

    return num;
}

/**
 * @brief Parses command line arguments.
 *
 * This function parses the command line arguments using the argp library.
 * It converts argument values to the appropriate data types and stores them
 * in the `arguments` structure.
 *
 * @param key The option key.
 * @param arg The option argument.
 * @param state The parser state.
 * @return Returns 0 if successful, otherwise returns an error code.
 */
static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    struct arguments *arguments = state->input;
    switch (key)
    {
    case 'i':
        arguments->interval = safe_convert_to_int(arg);
        break;
    case 't':
        arguments->time = safe_convert_to_int(arg);
        break;
    case 'n':
        arguments->mode = WATCH_BY_NAME;
        arguments->name = arg;
        break;
    case 'p':
        arguments->mode = WATCH_BY_PID;
        arguments->pid = safe_convert_to_int(arg);
        break;
    case ARGP_KEY_ARG:
        return 0;
    default:
        return ARGP_ERR_UNKNOWN;
    }

    // check for error
    if (arguments->interval == -1 || arguments->time == -1 || arguments->pid == -1)
    {
        return 1;
    }

    return 0;
}

/**
 * @brief Watches the descriptors of a process for a specified time interval.
 *
 * This function monitors the descriptors of the process with the given `pid`
 * for a specified `time_limit`. It prints the number of descriptors at regular
 * intervals defined by `interval`.
 *
 * @param pid The process ID to watch.
 * @param interval The interval at which to check the descriptors.
 * @param time_limit The maximum time to watch the process.
 * @return Returns 0 if successful, otherwise returns an error code.
 */
int watch(int pid, int interval, int time_limit)
{
    time_t end;
    time_t start = time(NULL);
    int descriptors;
    end = start + time_limit;

    if (!process_exists(pid))
    {
        printf("No process with pid [%d].\n", pid);
        return 1;
    }

    while (start < end)
    {
        descriptors = count_descriptors_by_pid(pid);

        switch (descriptors)
        {
        case -1:
            return 0;

        default:
            fprintf(stderr, "process %d: %d\n", pid, descriptors);
            sleep(interval);
            start = time(NULL);
            break;
        }
    }
    return 0;
}


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

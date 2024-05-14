#include <argp.h>
#include <stdio.h>
#include <unistd.h>
#include "monitor.h"
#include "utils.h"

error_t parse_opt(int key, char *arg, struct argp_state *state);

const char *argp_program_version = "fd-watcher 0.1";
static char doc[] = "File descriptor watcher";
static char args_doc[] = "[FILENAME]...";
static struct argp_option options[] = {
    {"time", 't', "TIME", OPTION_ARG_OPTIONAL,
     "How much time should the watchdog run.", 0},
    {"name", 'n', "NAME", OPTION_ARG_OPTIONAL,
     "The name of the process to monitor.", 0},
    {"pid", 'p', "PID", OPTION_ARG_OPTIONAL,
     "The pid of the process to monitor.", 0},
    {"interval", 'i', "INTERVAL", OPTION_ARG_OPTIONAL,
     "The interval between watch, in seconds.", 0},

    {0} // needs to be terminated by a zero
};
static struct argp argp = {options, parse_opt, args_doc, doc, 0, 0, 0};

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
error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    switch (key) {
    case 'i':
        arguments->interval = safe_convert_to_int(arg);
        break;
    case 't':
        arguments->time = safe_convert_to_int(arg);
        break;
    case 'n':
        arguments->name = arg;
        arguments->pid = get_pid_by_name(arg);
        break;
    case 'p':
        arguments->pid = safe_convert_to_int(arg);
        break;
    case 's':
        arguments->stats = 1;
        break;
    case ARGP_KEY_ARG:
        return 0;
    default:
        return ARGP_ERR_UNKNOWN;
    }

    // check for error
    if (arguments->interval == -1) {
        fprintf(stderr, "Failed to parse interval argument.\n");
        return 1;
    }
    if (arguments->time == -1) {
        fprintf(stderr, "Failed to parse time argument.\n");
        return 1;
    }
    if (arguments->pid == -1) {
        fprintf(stderr, "Failed to parse pid.\n");
        return 1;
    }

    return 0;
}

int main(int argc, char *argv[]) {

    int parsing_result;

    struct arguments arguments;
    arguments.name =
        (char *)""; // ugly casting to a pointer to avoid the warning
    arguments.pid = -1;
    arguments.time = 60;
    arguments.interval = 1;

    parsing_result = argp_parse(&argp, argc, argv, 0, 0, &arguments);

    if (parsing_result == 1) {
        fprintf(stderr, "Bad usage\n");
        return 1;
    }

    watch(arguments.pid, arguments.interval, arguments.time);

    return 0;
};

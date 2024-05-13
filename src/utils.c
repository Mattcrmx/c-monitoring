
#include <argp.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "utils.h"


#define BUF_SIZE 1024
char buffer[BUF_SIZE];

/**
 * @brief Check if the number of arguments matches the required number.
 *
 * @param args     The number of arguments provided.
 * @param required The number of arguments required.
 * @return 0 if the number of arguments matches the required number, 1 otherwise.
 */
int check_args(int args, int required)
{
    if (args != required)
    {
        printf("use: ./monitor_fd process_name\n");
        return 1;
    }
    else
    {
        return 0;
    }
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
error_t parse_opt(int key, char *arg, struct argp_state *state)
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
        arguments->name = arg;
        arguments->pid = get_pid_by_name(arg);
        break;
    case 'p':
        arguments->pid = safe_convert_to_int(arg);
        break;
    case ARGP_KEY_ARG:
        return 0;
    default:
        return ARGP_ERR_UNKNOWN;
    }

    // check for error
    if (arguments->interval == -1)
    {
        fprintf(stderr, "Failed to parse interval argument.\n");
        return 1;
    }
    if (arguments->time == -1)
    {
        fprintf(stderr, "Failed to parse time argument.\n");
        return 1;
    }
    if (arguments->pid == -1)
    {
        fprintf(stderr, "Failed to parse pid.\n");
        return 1;
    }

    return 0;
}

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
 * @brief Check if the given string represents a number.
 *
 * @param nb A pointer to the string to be checked.
 * @return 0 if the string represents a number, 1 otherwise.
 */
int is_number(char *nb)
{
    for (int i = 0; nb[i] != '\0'; ++i)
    {
        if (!isdigit(nb[i]))
        {
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Allocate memory safely using malloc.
 *
 * @param size The size of memory to allocate.
 * @return A pointer to the allocated memory.
 * @warning This function terminates the program with EXIT_FAILURE if memory allocation fails.
 */
void *safe_malloc(size_t size)
{
    void *ptr = malloc(size);
    if (ptr != NULL)
    {
        return ptr;
    }
    else
    {
        fprintf(stderr, "malloc of size %lu failed\n", size);
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Trim leading and trailing spaces from the given string.
 *
 * @param str A pointer to the string to be trimmed.
 * @return A pointer to the trimmed string.
 */
char *trim(char *str)
{
    char *end;

    // Trim leading space
    while (isspace((unsigned char)*str))
        str++;

    if (*str == 0)
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;

    end[1] = '\0';

    return str;
}

/**
 * @brief Get the process ID (PID) by process name.
 *
 * @param name The name of the process.
 * @return The PID of the process, or -1 if the process is not found.
 */
int get_pid_by_name(char name[])
{
    DIR *proc;
    proc = opendir("/proc");
    char *status_path = safe_malloc(50);
    struct dirent *de;

    while ((de = readdir(proc)) != NULL)
    {

        // skip files

        if (de->d_type != DT_DIR)
        {
            continue;
        }

        // skip . and ..
        if (is_number(de->d_name) == 1)
        {
            continue;
        }

        sprintf(status_path, "/proc/%s/status", de->d_name);
        FILE *fp;
        fp = fopen(status_path, "r");
        long length;

        if (fp != NULL)
        {
            // read the file's content
            while (fgets(buffer, BUF_SIZE, fp) != NULL)
            {
                length = strlen(buffer);
                // Trim new line character from last if exists.
                buffer[length - 1] = buffer[length - 1] == '\n' ? '\0' : buffer[length - 1];

                // find the name
                char *res = strstr(buffer, "Name");
                if (res)
                {
                    char proc_name[255];
                    strcpy(proc_name, trim(&buffer[5]));

                    if (strcmp(name, proc_name) == 0)
                    {
                        printf("name : %s - pid: %s\n", proc_name, de->d_name);
                        return atoi(de->d_name);
                    }
                }
            }
        }
        else
        {
            continue;
        }
    }

    free(status_path);

    return -1;
}

/**
 * @brief Check if a process with the given PID exists.
 *
 * @param pid The PID of the process to check.
 * @return 1 if the process exists, 0 otherwise.
 */
int process_exists(int pid)
{
    char *proc_path = safe_malloc(16);
    sprintf(proc_path, "/proc/%d/fd", pid);

    DIR *proc_dir;
    proc_dir = opendir(proc_path);
    if (proc_dir)
    {
        closedir(proc_dir);
        free(proc_path);
        return 1;
    }
    else
    {
        free(proc_path);
        return 0;
    }
}
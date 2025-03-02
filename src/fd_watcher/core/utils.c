
#include <argp.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "utils.h"

#define MAX_STATUS_PATH_LENGTH 50
#define MAX_SNPRINTF_OUT_LENGTH 269
// /proc/<pid>/fd arbitrary truncated up to 9999999
#define MAX_PROC_PATH_LENGTH 16
#define PROC_NAME_INDEX 5
#define BUF_SIZE 1024

char buffer[BUF_SIZE];

static int is_number(char *nb);
static char *trim(char *str);

/**
 * @brief Converts a string representation of an integer to an integer value
 * safely.
 *
 * This function converts the input string `pid` to an integer value. It checks
 * for invalid input, overflow, and underflow.
 *
 * @param pid Pointer to the string to convert.
 * @return Returns the integer value if successful, otherwise returns -1 and
 * prints an error message to stderr.
 */
int safe_convert_to_int(char *pid) {
    // helper function to be sure that the provided pid is well formatted.
    // courtesy of
    // https://stackoverflow.com/questions/8871711/atoi-how-to-identify-the-difference-between-zero-and-error
    long lnum;
    int num;
    char *end;

    errno = 0;

    lnum = strtol(pid, &end, 10);

    if (end == pid) {
        // if no characters were converted these pointers are equal
        fprintf(stderr, "ERROR: can't convert string to number\n");
        num = -1;
    } else if ((lnum == LONG_MAX || lnum == LONG_MIN) && errno == ERANGE) {
        // If sizeof(int) == sizeof(long), we have to explicitly check for
        // overflows
        fprintf(stderr, "ERROR: number out of range for LONG\n");
        num = -1;
    } else if ((lnum > INT_MAX) || (lnum < INT_MIN)) {
        // Because strtol produces a long, check for overflow
        fprintf(stderr, "ERROR: number out of range for INT\n");
        num = -1;
    } else {
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
static int is_number(char *nb) {
    for (int i = 0; nb[i] != '\0'; ++i) {
        if (!isdigit(nb[i])) {
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
 * @warning This function terminates the program with EXIT_FAILURE if memory
 * allocation fails.
 */
void *safe_malloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr != NULL) {
        return ptr;
    } else {
        fprintf(stderr, "malloc of size %zu failed\n", size);
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Trim leading and trailing spaces from the given string.
 *
 * @param str A pointer to the string to be trimmed.
 * @return A pointer to the trimmed string.
 */
static char *trim(char *str) {
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
 * @brief Get the pid from the process name.
 *
 * @param name The name of the process.
 * @return The pid of the process, or -1 if the process is not found.
 */
int get_pid_by_name(char name[]) {
    DIR *proc;
    struct dirent *de;

    proc = opendir("/proc");
    char *status_path = safe_malloc(MAX_SNPRINTF_OUT_LENGTH);

    while ((de = readdir(proc)) != NULL) {
        FILE *fp;

        // skip files and skip . and ..
        if (de->d_type != DT_DIR || is_number(de->d_name) == 1) {
            continue;
        }

        snprintf(status_path, MAX_SNPRINTF_OUT_LENGTH, "/proc/%s/status",
                 de->d_name);
        fp = fopen(status_path, "r");

        if (fp != NULL) {
            // read the file's content
            while (fgets(buffer, BUF_SIZE, fp) != NULL) {
                long length;
                length = strlen(buffer);
                // Trim new line character from last if exists.
                buffer[length - 1] =
                    buffer[length - 1] == '\n' ? '\0' : buffer[length - 1];

                // find the name
                char *res = strstr(buffer, "Name");
                if (res) {
                    char proc_name[255];
                    strcpy(proc_name, trim(&buffer[PROC_NAME_INDEX]));

                    if (strcmp(name, proc_name) == 0) {
                        free(status_path);
                        return atoi(de->d_name);
                    }
                }
            }
        } else {
            continue;
        }
    }

    free(status_path);
    return -1;
}

/**
 * @brief Get the name of a process by its process ID.
 *
 * @param pid The process ID.
 * @return The process name, or NULL if not found.
 */
char *get_name_from_pid(int pid) {
    FILE *fp;
    char status_path[MAX_STATUS_PATH_LENGTH];
    sprintf(status_path, "/proc/%d/status", pid);
    fp = fopen(status_path, "r");

    if (fp) {
        while (fgets(buffer, BUF_SIZE, fp) != NULL) {
            long length;
            length = strlen(buffer);
            // Trim new line character from last if exists.
            buffer[length - 1] =
                buffer[length - 1] == '\n' ? '\0' : buffer[length - 1];

            // find the name
            char *res = strstr(buffer, "Name");
            if (res) {
                char *proc_name = safe_malloc(255);
                strcpy(proc_name, trim(&buffer[5]));
                return proc_name;
            }
        }
        // If the loop completes without finding the name, close the file and
        // return NULL
        fclose(fp);
        fprintf(stderr, "Error: Process name not found in status file.\n");
        return NULL;
    } else {
        fprintf(stderr, "Error opening status file.\n");
        return NULL;
    }
}

/**
 * @brief Check if a process with the given PID exists.
 *
 * @param pid The PID of the process to check.
 * @return 1 if the process exists, 0 otherwise.
 */
int process_exists(int pid) {
    char *proc_path = safe_malloc(MAX_PROC_PATH_LENGTH);
    sprintf(proc_path, "/proc/%d/fd", pid);

    DIR *proc_dir;
    proc_dir = opendir(proc_path);
    if (proc_dir) {
        closedir(proc_dir);
        free(proc_path);
        return 1;
    } else {
        free(proc_path);
        return 0;
    }
}

#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "utils.h"
#include <time.h>
#include <ctype.h>
#include <unistd.h>

int get_pid_by_name(char name[]);
int count_descriptors_by_name(char name[]);
int count_descriptors_by_pid(int pid);
int watch(int pid, int interval, int time_limit);

#define BUF_SIZE 1024
char buffer[BUF_SIZE];

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
                /* Total character read count */
                length = strlen(buffer);
                // Trim new line character from last if exists.
                buffer[length - 1] = buffer[length - 1] == '\n' ? '\0' : buffer[length - 1];

                // find the name
                char *res = strstr(buffer, "Name");
                if (res)
                {
                    char proc_name[255];
                    strcpy(proc_name, trim(&buffer[5]));
                    // printf("name %s - proc_name %s - pid %d\n", name, proc_name, i);

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

/**
 * @brief Count the number of file descriptors associated with a process by process name.
 *
 * @param name The name of the process.
 * @return The number of file descriptors associated with the process.
 */
int count_descriptors_by_name(char name[])
{
    // wrapper around the count_descriptors_by_pid function
    int pid = get_pid_by_name(name);
    int descriptors = count_descriptors_by_pid(pid);

    return descriptors;
}

/**
 * @brief Count the number of file descriptors associated with a process by PID.
 *
 * @param pid The PID of the process.
 * @return The number of file descriptors associated with the process.
 */
int count_descriptors_by_pid(int pid)
{
    int nb_fd = 0;
    char *proc_path = safe_malloc(9 + 6 + 1);

    sprintf(proc_path, "/proc/%d/fd", pid);

    DIR *proc_dir;
    proc_dir = opendir(proc_path);

    if (process_exists(pid))
    {
        struct dirent *de;
        while ((de = readdir(proc_dir)) != NULL)
        {
            nb_fd++;
        }

        closedir(proc_dir);
    }
    else
    {
        free(proc_path);
        return -1;
    }

    free(proc_path);
    return nb_fd;
}

int watch(int pid, int interval, int time_limit);

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

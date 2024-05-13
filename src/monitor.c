#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include "utils.h"
#include "monitor.h"

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

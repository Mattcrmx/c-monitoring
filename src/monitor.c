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

struct DescriptorsArray
{
    int *descriptors;
    int *timestamps;
    int length
};

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
    int descriptors;
    time_t start = time(NULL);
    time_t end = start + time_limit;

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

struct DescriptorsArray *generate_fd_stats(int pid, int interval, int time_limit)
{
    int nb_descriptors;
    time_t start = time(NULL);
    time_t end = start + time_limit;
    long nb_slots = (end - start) / interval + 1;
    int *descriptors = (int)safe_malloc(nb_slots * sizeof(int));
    int *timestamps = (int)safe_malloc(nb_slots * sizeof(int));
    struct DescriptorsArray *desc_array;

    if (!process_exists(pid))
    {
        printf("No process with pid [%d].\n", pid);
        return 1;
    }

    for (size_t i = 0; i < nb_slots; i++)
    {
        nb_descriptors = count_descriptors_by_pid(pid);

        switch (nb_descriptors)
        {
        case -1:
            // return empty array
            free(descriptors);
            desc_array->descriptors = NULL;
            desc_array->timestamps = NULL;
            desc_array->length = 0;
            return desc_array;

        default:
            descriptors[i] = nb_descriptors;
            timestamps[i] = start + i * interval;
            sleep(interval);
            break;
        }
    }

    // return pointer to descriptors
    desc_array->descriptors = descriptors;
    desc_array->timestamps = timestamps;
    desc_array->length = nb_descriptors;

    return desc_array;
}

int write_stats_to_csv(struct DescriptorsArray *desc_array, char *process_name)
{
    char filepath[255];
    sprintf(filepath, "./fd_stats_%s.csv", process_name);
    FILE *fp;
    fp = fopen(filepath, "w");

    if (fp != NULL)
    {
        for (size_t i = 0; i < desc_array->length; i++)
        {
            fprintf(fp, "%d, %d\n", desc_array->timestamps[i], desc_array->descriptors[i]);
        }
    }
    else
    {
        flcose(fp);
        fprintf(stderr, "failed to create %s", filepath);
        return 0;
    }

    flcose(fp);

    return 1;
}
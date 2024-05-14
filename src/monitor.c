#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "monitor.h"
#include "utils.h"

static DescriptorsArray *generate_fd_stats(int pid, int interval,
                                           int time_limit);
static int write_stats_to_csv(DescriptorsArray *desc_array, char *process_name);
static int logger_watcher(int pid, int interval, int time_limit);
static int count_descriptors_by_pid(int pid);

/**
 * @brief Count the number of file descriptors associated with a process by PID.
 *
 * @param pid The PID of the process.
 * @return The number of file descriptors associated with the process.
 */
static int count_descriptors_by_pid(int pid) {
    int nb_fd = 0;
    char *proc_path = safe_malloc(16);

    sprintf(proc_path, "/proc/%d/fd", pid);

    DIR *proc_dir;
    proc_dir = opendir(proc_path);

    if (process_exists(pid)) {
        struct dirent *de;
        while ((de = readdir(proc_dir)) != NULL) {
            nb_fd++;
        }

        closedir(proc_dir);
    } else {
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
int watch(Arguments arguments) {
    int ret_code;
    DescriptorsArray *desc_array;

    switch (arguments.mode) {
    case LOGGER:
        ret_code =
            logger_watcher(arguments.pid, arguments.interval, arguments.time);
        break;
    case STATISTICAL:
        desc_array = generate_fd_stats(arguments.pid, arguments.interval,
                                       arguments.time);
        ret_code = write_stats_to_csv(desc_array, arguments.name);
        break;
    }
    return ret_code;
}

static int logger_watcher(int pid, int interval, int time_limit) {
    int descriptors;
    time_t start = time(NULL);
    time_t end = start + time_limit;

    if (!process_exists(pid)) {
        printf("No process with pid [%d].\n", pid);
        return 1;
    }

    while (start < end) {
        descriptors = count_descriptors_by_pid(pid);

        if (descriptors == -1) {
            return 0;
        }

        fprintf(stderr, "process %d: %d\n", pid, descriptors);
        sleep(interval);
        start = time(NULL);
    }
    return 0;
}

static DescriptorsArray *generate_fd_stats(int pid, int interval,
                                           int time_limit) {
    int nb_descriptors;
    time_t start = time(NULL);
    time_t end = start + time_limit;
    long nb_slots = (end - start) / interval + 1;
    int *descriptors = (int *)safe_malloc(nb_slots * sizeof(int));
    int *timestamps = (int *)safe_malloc(nb_slots * sizeof(int));
    DescriptorsArray *desc_array;

    desc_array = (DescriptorsArray *)safe_malloc(sizeof(DescriptorsArray));

    desc_array->descriptors = NULL;
    desc_array->timestamps = NULL;
    desc_array->length = 0;

    if (!process_exists(pid)) {
        // avoid generating stats and free
        printf("No process with pid [%d].\n", pid);
        free(descriptors);
        free(timestamps);
        free(desc_array);
        return NULL;
    }

    for (int i = 0; i < nb_slots; i++) {
        nb_descriptors = count_descriptors_by_pid(pid);

        switch (nb_descriptors) {
        case -1:
            free(descriptors);
            free(timestamps);
            free(desc_array);
            return NULL;

        default:
            // increment both tick and descriptors
            descriptors[i] = nb_descriptors;
            timestamps[i] = start + i * interval;
            sleep(interval);
            break;
        }
    }

    desc_array->descriptors = descriptors;
    desc_array->timestamps = timestamps;
    desc_array->length = nb_slots;

    return desc_array;
}

static int write_stats_to_csv(DescriptorsArray *desc_array,
                              char *process_name) {
    // generate file path and write csv
    char filepath[255];
    sprintf(filepath, "./fd_stats_%s.csv", process_name);
    FILE *fp;
    fp = fopen(filepath, "w");

    if (fp != NULL) {
        for (int i = 0; i < desc_array->length; i++) {
            fprintf(fp, "%d, %d\n", desc_array->timestamps[i],
                    desc_array->descriptors[i]);
        }
    } else {
        fclose(fp);
        fprintf(stderr, "failed to create %s", filepath);
        return 0;
    }

    fclose(fp);

    return 1;
}
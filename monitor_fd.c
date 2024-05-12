#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

int check_args(int args, int required);
void *safe_malloc(size_t size);
char *trim(char *str);
int get_pid_by_name(char name[]);
int count_descriptors_by_name(char name[]);
int count_descriptors_by_pid(int pid);
int is_number(char *nb);
int process_exists(int pid);

#define BUF_SIZE 1024
char buffer[BUF_SIZE];

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

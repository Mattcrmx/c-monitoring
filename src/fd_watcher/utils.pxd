cdef extern from "stddef.h":
    ctypedef size_t size_t

cdef extern from "core/utils.h":
    cdef enum MODE:
        LOGGER
        STATISTICAL

    cdef struct Arguments:
        char *name
        int pid
        int time
        int interval
        int stats
        MODE mode

    int safe_convert_to_int(char *pid)
    int process_exists(int pid)
    int get_pid_by_name(char name[])
    void *safe_malloc(size_t size)
    char *get_name_from_pid(int pid)

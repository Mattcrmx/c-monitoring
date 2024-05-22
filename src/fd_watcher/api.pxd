cdef extern from "core/utils.h":
    cdef enum MODE:
        LOGGER
        STATISTICAL

    cdef struct Arguments:
        char *name
        int pid
        int time
        float interval
        int stats
        MODE mode

cdef extern from "core/monitor.h":
    cdef struct DescriptorsArray:
        int *descriptors
        long *timestamps
        int length

cdef extern from "core/api.h":
    int literal_watch(float interval, int time, char *name, int pid, int stats)
    Arguments *new_args(int interval, int time, char *name, int pid, int stats);
    Arguments *new_empty_args();
    DescriptorsArray *new_desc_array();
    DescriptorsArray generate_fd_stats_by_value(int pid, float interval, int time_limit);

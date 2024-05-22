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

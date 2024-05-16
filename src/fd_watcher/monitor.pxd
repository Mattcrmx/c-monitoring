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

cdef extern from "core/monitor.h":
    cdef struct DescriptorsArray:
        int *descriptors
        int *timestamps
        int length

    int watch(Arguments arguments)
    DescriptorsArray *new_desc_array()

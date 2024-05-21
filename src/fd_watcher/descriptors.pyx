# The descriptors bindings file.
from cython.cimports import api, monitor
from cython.cimports.utils import (
    get_name_from_pid,
    get_pid_by_name,
)


cpdef char* py_get_name_from_pid(int pid):
    """Get the process name for the pid."""
    return get_name_from_pid(pid)

cpdef int py_get_pid_from_name(char* name):
    """Get the pid from the process name."""
    return get_pid_by_name(name)

cpdef int py_watch(
    int interval,
    int time,
    bytes name,
    int pid,
    int stats
):
    """The watchdog method for watching file descriptors leak.

    Args:
        interval: the interval at which to capture data points
        time: the total monitoring time
        name: the process name
        pid: the pid
        stats: whether to enable statistic collection.

    Returns:
        The descriptor array containing the leak statistics.
    """
    cdef char* cname = name

    return api.literal_watch(interval, time, cname, pid, stats)

cpdef dict generate_descriptor_array(int pid, int interval, int time_limit):
    """Generate execution statistics.

    Args:
        pid:
        interval:
        time_limit:

    Returns:
        The wrapper class for the statistics.
    """
    cdef dict *arr = monitor.generate_fd_stats(pid, interval, time_limit)
    return arr

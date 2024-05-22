"""The descriptors bindings file."""
from cython.cimports import api, monitor
from cython.cimports.utils import (
    get_name_from_pid,
    get_pid_by_name,
)
from cython.cimports.monitor import DescriptorsArray


cdef class PyDescriptorsArray:
    """The C Descriptor array wrapper"""
    cdef DescriptorsArray desc_array

    def __init__(self, pid, interval, time_limit):
        """The initialization method"""
        self.desc_array = api.generate_fd_stats_by_value(pid, interval, time_limit)

    @property
    def length(self):
        """The array length."""
        return self.desc_array.length

    @property
    def descriptors(self):
        """The array length."""
        return [self.desc_array.descriptors[i] for i in range(self.length)]

    @property
    def timestamps(self):
        """The array length."""
        return [self.desc_array.timestamps[i] for i in range(self.length)]


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

cpdef PyDescriptorsArray generate_descriptor_array(int pid, int interval, int time_limit):
    """Generate execution statistics.

    Args:
        pid: the process id
        interval: the interval at which to capture data points
        time_limit: the time limit of the monitoring

    Returns:
        The wrapper class for the statistics.
    """
    return PyDescriptorsArray(pid, interval, time_limit)

"""The decriptors bindings file."""

import cython
from cython.cimports import api, monitor
from cython.cimports.utils import (
    get_name_from_pid,
    get_pid_by_name,
)

DescriptorArrayWrapper = cython.struct(
    length=cython.int, descriptors=cython.pointer(int), timestamps=cython.pointer(int)
)


def py_get_name_from_pid(pid: cython.int) -> cython.p_char:
    """Get the process name for the pid."""
    return get_name_from_pid(pid)


def py_get_pid_from_name(name: cython.p_char) -> cython.int:
    """Get the pid from the process name."""
    return get_pid_by_name(name)


def py_watch(
    interval: cython.int,
    time: cython.int,
    name: cython.bytes,
    pid: cython.int,
    stats: cython.int,
) -> int:
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
    name: cython.p_char

    return api.literal_watch(interval, time, name, pid, stats)


def generate_descriptor_array(
    pid: cython.int, interval: cython.int, time_limit: cython.int
) -> DescriptorArrayWrapper:
    """Generate execution statistics.

    Args:
        pid: the process id
        interval: the interval at which to capture data points
        time_limit: the total monitoring time

    Returns:
        The wrapper class for the statistics.
    """
    arr = monitor.generate_fd_stats(pid, interval, time_limit)
    return DescriptorArrayWrapper(arr.length, arr.descriptors, arr.timestamps)

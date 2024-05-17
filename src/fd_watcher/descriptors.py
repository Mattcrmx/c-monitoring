"""The decriptors bindings file."""

import cython
from cython.cimports import monitor
from cython.cimports.libc.stdlib import free, malloc
from cython.cimports.libc.string import memcpy
from cython.cimports.utils import (
    Arguments,
    get_name_from_pid,
    get_pid_by_name,
    new_empty_args,
)


@cython.cclass
class _CDescriptorsArray:
    """The Descriptor array struct wrapper."""

    _c_descriptors_array: cython.pointer(monitor.DescriptorsArray)

    def __cinit__(self):
        self._c_descriptors_array = monitor.new_desc_array()

    @property
    def length(self) -> int:
        return self._c_descriptors_array.length


@cython.cclass
class _CArguments:
    """The Python wrapper for the C arguments struct."""

    c_struct: cython.pointer(Arguments)

    def __cinit__(
        self,
        interval: cython.int,
        time: cython.int,
        name: cython.p_char,
        pid: cython.int,
        mode: cython.p_char,
    ):
        """Initialization method."""
        self.c_struct = new_empty_args()
        self.c_struct.interval = interval
        self.c_struct.time = time
        encoded_name = name
        self.c_struct.name = cython.cast(cython.p_char, malloc(len(encoded_name) + 1))
        memcpy(
            self.c_struct.name,
            cython.cast(cython.p_char, encoded_name),
            len(encoded_name),
        )
        self.c_struct.name[len(encoded_name)] = b"\0"  # Null-terminate the C string

        self.c_struct.pid = pid
        # self.c_struct.mode = mode

    def __dealloc__(self):
        """Free allocated memory."""
        free(self.c_struct)

    @property
    def name(self):
        return self.c_struct.name

    @property
    def interval(self):
        return self.c_struct.interval

    @property
    def time(self):
        return self.c_struct.time

    @property
    def pid(self):
        return self.c_struct.pid


def py_get_name_from_pid(pid: cython.int) -> cython.p_char:
    """Get the process name for the pid."""
    return get_name_from_pid(pid)


def py_get_pid_from_name(name: cython.p_char) -> cython.int:
    """Get the pid from the process name."""
    return get_pid_by_name(name)


def py_watch(args: _CArguments) -> _CDescriptorsArray:
    """The watchdog method for watching file descriptors leak.

    Args:
        args: C struct.

    Returns:
        The descriptor array containing the leak statistics.
    """
    return monitor.watch(args)

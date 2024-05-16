"""The decriptors bindings file."""

from __future__ import annotations

from dataclasses import dataclass
from enum import Enum
from typing import Literal

import cython
from cython.cimports import monitor
from cython.cimports.monitor import watch
from cython.cimports.utils import Arguments, get_name_from_pid, get_pid_by_name


class WatchMode(str, Enum):
    """Watcher modes."""

    LOGGER = "logger"
    STATISTICAL = "statistical"


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

    def __cinit__(self, interval: int, time: int, name: str, pid: int, mode: str):
        """Initialization method."""
        self.c_struct.interval = interval
        self.c_struct.time = time
        self.c_struct.name = name
        self.c_struct.pid = pid
        self.c_struct.mode = mode


@dataclass
class FdWatcher:
    """The File Descriptor Watcher base wrapper class."""

    interval: int
    time: int
    name: str | None
    pid: int | None

    def __post_init__(self):
        """Post Initialization method."""
        if not self.name and self.pid:
            raise ValueError("Must provide one of pid or name")
        if self.pid is not None and not isinstance(self.pid, int):
            raise ValueError(f"Wrong value provided for pid: {self.pid}")

        if self.name and not self.pid:
            self.pid = self._get_pid_from_name()
        if self.pid and not self.name:
            self.name = self._get_name_from_pid()

    def _get_name_from_pid(self) -> str:
        """Get the process name for the pid."""
        return get_name_from_pid(self.pid)

    def _get_pid_from_name(self):
        """Get the pid from the process name."""
        return get_pid_by_name(self.name)

    def watch(
        self, mode: Literal["logger", "statistical"]
    ) -> _CDescriptorsArray | None:
        """The watchdog method for watching file descriptors leak.

        Args:
            mode: the watching mode.

        Returns:
            The descriptor array containing the leak statistics.
        """
        args = _CArguments(
            interval=self.interval,
            time=self.time,
            name=self.name,
            pid=self.pid,
            mode=mode,
        )
        desc_array = None
        if mode == WatchMode.STATISTICAL:
            desc_array = watch(args.c_struct)
        else:
            watch(args.c_struct)

        return desc_array

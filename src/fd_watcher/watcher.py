"""The File descriptor watcher module."""

from __future__ import annotations

from dataclasses import dataclass
from enum import Enum
from typing import Literal

from fd_watcher.descriptors import (
    py_get_name_from_pid,
    py_get_pid_from_name,
    py_watch,
)


class WatchMode(str, Enum):
    """Watcher modes."""

    LOGGER = "logger"
    STATISTICAL = "statistical"


@dataclass
class FdWatcher:
    """The File Descriptor Watcher base wrapper class."""

    interval: int
    time: int
    name: str | None = None
    pid: int | None = None

    def __post_init__(self):
        """Post Initialization method."""
        if not self.name and not self.pid:
            raise ValueError("Must provide one of pid or name")
        if self.pid is not None and not isinstance(self.pid, int):
            raise ValueError(f"Wrong value provided for pid: {self.pid}")

        if self.name and not self.pid:
            self.pid = py_get_pid_from_name(self._c_name)
        if self.pid and not self.name:
            self.name = py_get_name_from_pid(self.pid).decode("utf-8")

    @property
    def _c_name(self) -> bytes:
        """C compatible name."""
        return self.name.encode("utf-8")

    def watch(self, mode: Literal["logger", "statistical"]):
        """The watchdog method for watching file descriptors leak.

        Args:
            mode: the watching mode.

        Returns:
            The descriptor array containing the leak statistics.
        """
        py_watch(
            interval=self.interval,
            time=self.time,
            name=self._c_name,
            pid=self.pid,
            stats=0 if mode == WatchMode.LOGGER else 1,
        )

"""The File descriptor watcher module."""

from __future__ import annotations

from dataclasses import dataclass
from enum import Enum
from typing import Literal

import matplotlib.pyplot as plt

from fd_watcher.descriptors import (
    PyDescriptorsArray,
    generate_descriptor_array,
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

    name: str | None = None
    pid: int | None = None
    desc_array: PyDescriptorsArray = None

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

    def _collect_stats(self, interval: float, time: int):
        """Statistics collection."""
        self.desc_array = generate_descriptor_array(self.pid, interval, time)

    def plot(
        self,
        figsize: tuple[int, int] = (10, 6),
        save_path: str = "./fd_monitoring.png",
    ):
        """Plot the file descriptors monitoring.

        Args:
            figsize: the figure size for matplotlib
            save_path: the path to save the figure
        """
        timestamps = self.desc_array.timestamps
        # rescale timestamps
        timestamps = [(x - timestamps[0]) / 1000 for x in timestamps]
        descriptors = self.desc_array.descriptors

        plt.figure(figsize=figsize)
        plt.plot(
            timestamps,
            descriptors,
            marker="o",
            linestyle="-",
            color="b",
            label="File descriptors per tick",
        )
        plt.title("File descriptors monitoring", fontsize=16, fontweight="bold")
        plt.xlabel("Time (s)", fontsize=14)
        plt.ylabel("File descriptors", fontsize=14)
        plt.grid(True, which="both", linestyle="--", linewidth=0.5)
        plt.legend(loc="upper left", fontsize=12)
        plt.gca().set_facecolor("whitesmoke")
        plt.savefig(save_path, dpi=300, bbox_inches="tight")
        plt.show()

    def watch(
        self,
        mode: Literal["logger", "statistical"],
        interval: float = 1,
        time: int = 60,
    ) -> None:
        """The watchdog method for watching file descriptors leak.

        Args:
            time: the total time frame of the monitoring
            interval: the interval at which to capture data points
            mode: the watching mode.

        Returns:
            The descriptor array containing the leak statistics.
        """
        if mode == WatchMode.LOGGER:
            py_watch(
                interval=interval, time=time, name=self._c_name, pid=self.pid, stats=0
            )
        elif mode == WatchMode.STATISTICAL:
            self._collect_stats(interval, time)
            self.plot()

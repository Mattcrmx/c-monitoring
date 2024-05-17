"""Python plotting API for the package."""

from fd_watcher.watcher import FdWatcher

if __name__ == "__main__":
    watcher = FdWatcher(1, 5, "python3")
    watcher.watch("logger")

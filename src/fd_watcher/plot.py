"""Python plotting API for the package."""

from fd_watcher.watcher import FdWatcher

if __name__ == "__main__":
    watcher = FdWatcher(1, 5, pid=106995)
    watcher.watch("statistical")

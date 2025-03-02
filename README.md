<p align="center">
<a href="https://ibb.co/yfGCVxW"><img src="https://i.ibb.co/YRYSbVp/image-9.png" alt="fd-watcher-logo" width="40%" height="40%"></a>
</p>


# fd-watcher
A simple file descriptors watchdog.

# Building
Simply run the Makefile:
```bash
make
```
# Documentation
Documentation is built with doxygen. Simply run
```bash
doxygen Doxyfile
```

# Usage
```bash
./fd_watcher --pid=42069
```
or

```bash
./fd_watcher --name=mylittleprocess
```

Options can be specified:
```bash
-p --pid: the pid of the process to monitor
-n --name: the name of the process to monitor
-t --time: the time limit of the watchdog
-i --interval: the interval at which the watchdog will watch the descriptors
-s --stats: enable statistics collection, and csv export of the file descriptors monitoring
```

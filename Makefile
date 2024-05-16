CC=gcc
DEPS= src/fd_watcher/core/monitor.h src/fd_watcher/core/utils.h
OBJ= src/fd_watcher/core/monitor.c src/fd_watcher/core/utils.c src/fd_watcher/core/main.c
CFLAGS = -g \
		-W \
		-Wall \
		-Wshadow \
		-Wwrite-strings \
		-Wstrict-prototypes \
		-Wmissing-prototypes \
		-Wmissing-declarations \
		-D_GNU_SOURCE \
		-D_REENTRANT \
		-D_LINUX_ \

%.o: src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

fd-watcher: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

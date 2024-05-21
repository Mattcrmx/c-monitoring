CC=gcc
DEPS= src/fd_watcher/core/monitor.h src/fd_watcher/core/utils.h
OBJ= src/fd_watcher/core/monitor.c src/fd_watcher/core/utils.c src/fd_watcher/core/main.c
CFLAGS = -g \
		-W \
		-Wall \
		-Wextra \
		-Wpedantic \
		-Wshadow \
		-Wwrite-strings \
		-Wstrict-prototypes \
		-Wmissing-prototypes \
		-Wmissing-declarations \
		-Wno-unused-parameter \
		-Wshadow \
		-Wold-style-definition \
		-Wredundant-decls \
		-Wnested-externs \
		-Wmissing-include-dirs \
		-D_GNU_SOURCE \
		-D_REENTRANT \
		-D_LINUX_ \
		-O2

%.o: src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

fd-watcher: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

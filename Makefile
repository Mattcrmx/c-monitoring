CC=gcc
DEPS= src/monitor_fd.h src/utils.h
OBJ= src/monitor_fd.c src/utils.c src/main.c
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

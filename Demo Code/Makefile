CFLAGS=-std=gnu99
CC=gcc

all: hw print-opts print-args

hw: hello-world.c
	$(CC) $(CFLAGS) -o hello-world hello-world.c

print-args: print-args.c
	$(CC) $(CFLAGS) -o print-args print-args.c

print-opts: print-opts.c
	$(CC) $(CFLAGS) -o print-opts print-opts.c

clean:
	$(RM) -f hello-world print-args print-opts
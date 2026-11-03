CC := gcc

all: empty part1

empty: empty.c
	$(CC) -o empty empty.c

part1: part1.c
	$(CC) -o part1 part1.c

# Regenerates both trace files. part1.trace should have five more lines.
trace: all
	strace -o empty.trace ./empty
	strace -o part1.trace ./part1
	wc -l empty.trace part1.trace

clean:
	rm -f empty part1

.PHONY: all trace clean

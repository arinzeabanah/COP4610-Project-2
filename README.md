# Project 2: Elevator Kernel Module

COP4610: Operating Systems
Group 14: Jonathan Norton, Robyn Wilson, Arinze Abanah

## Division of Labor

### Before the project (planned, submitted 10/9/26):

| Part | Description | Members |
|------|-------------|---------|
| 1  | System call tracing | Robyn Wilson, Arinze Abanah |
| 2  | Timer kernel module | Jonathan Norton, Robyn Wilson |
| 3a | Adding system calls | Jonathan Norton, Arinze Abanah |
| 3b | Kernel compilation | Jonathan Norton, Arinze Abanah |
| 3c | Threads | Jonathan Norton, Robyn Wilson |
| 3d | Linked lists | Jonathan Norton, Robyn Wilson |
| 3e | Mutexes | Jonathan Norton, Arinze Abanah |
| 3f | Scheduling algorithm | Jonathan Norton, Robyn Wilson, Arinze Abanah |

### After the project (actual):

All three members worked together on most parts and helped each other as needed.
The assignments above were the starting point and shifted as the work progressed.

| Member | Contributions |
|--------|---------------|
| Jonathan Norton | Timer module, system calls, kernel build, elevator core logic |
| Robyn Wilson | System call tracing, timer module, threads, linked lists |
| Arinze Abanah | System call tracing, system calls, kernel build, mutexes, testing |

## File Listing

```
.

├── part1/
│   ├── Makefile
│   ├── empty.c
│   ├── empty.trace
│   ├── part1.c
│   └── part1.trace
├── part2/
│   ├── src/
│   │   └── my_timer.c
│   └── Makefile
├── part3/
│   ├── src/
│   │   └── elevator.c
│   ├── Makefile
│   └── syscalls.c
├── .gitignore
├──  Makefile
├──  README.md

```

- `part1/`: empty program and a program with exactly five added system calls,
  plus their strace output.
- `part2/`: `my_timer` kernel module that creates `/proc/timer`.
- `part3/`: `elevator` kernel module, the three system calls
  (`start_elevator`, `issue_request`, `stop_elevator`) and the proc entry
  `/proc/elevator`.

## How to Compile and Run

Requirements
`gcc`, `make`, and `strace`
Kernel headers for the running kernel
Part 3 requires Linux 6.16.12 built with system calls 548-550 (see Part 3 below).
Parts 1 and 2 build on any recent kernel.
Building everything
From the repository root:
```
make
```
This builds all three parts. `make clean` removes executables and module build files.
Part 1: System call tracing
```
cd part1
make
make trace
```
`make` builds the `empty` and `part1` executables. `make trace` runs both under `strace`,
writes `empty.trace` and `part1.trace`, and prints their line counts. `part1.trace` has
five more lines than `empty.trace`.
Part 2: Timer kernel module
```
cd part2
make
sudo insmod my_timer.ko
cat /proc/timer
sleep 1
cat /proc/timer
sudo rmmod my_timer
```
`make` builds `my_timer.ko` in `part2/`. The first read of `/proc/timer` prints the current
time. Each later read also prints the time elapsed since the previous read.
Part 3: Elevator kernel module
Kernel setup (one time). In the Linux 6.16.12 source:
Copy `part3/syscalls.c` to `syscalls/syscalls.c` and create `syscalls/Makefile`
containing `obj-y := syscalls.o`.
Add entries 548, 549, and 550 to `arch/x86/entry/syscalls/syscall_64.tbl`.
Declare the three system calls in `include/linux/syscalls.h`.
Change `core-y :=` to `core-y := syscalls/` in the top-level `Makefile`.
Run `make -j$(nproc)`, `sudo make modules_install`, `sudo make install`, and reboot.
`uname -r` should print `6.16.12`.
Check the system calls:
```
cd part3/tests/syscheck
make
sudo insmod syscheck.ko
make run
sudo rmmod syscheck
```
Build and load the elevator:
```
cd part3
make
sudo insmod elevator.ko
```
`make` builds `elevator.ko` in `part3/`. The elevator starts in the OFFLINE state.
Run it (two terminals make this easier to watch):
```
cd part3/tests/elevator
make
./consumer --start
./producer 20
watch -n 1 cat /proc/elevator
```
`./producer N` issues N random requests. Requests with a floor outside 1-5 return 1.
Stop and unload:
```
./consumer --stop
sudo rmmod elevator
```
After `--stop`, the elevator delivers the pets on board, picks up no one new, then goes
OFFLINE. Wait for OFFLINE before running `rmmod`.

## Development Log

### Jonathan Norton

- Wrote the my_timer module and the /proc/timer read handler
- Added system calls 548, 549 and 550 to the kernel source
- Compiled and installed the custom kernel
- Implemented the elevator kthread loop and floor linked lists

### Robyn Wilson

- Format the README.md 
- Worked on the my_timer module
- Implemented pet structs and per-floor linked lists
- Worked on the /proc/elevator output

### Arinze Abanah

- Created empty.c and part1.c and captured the strace output
- Added the system calls and compiled the kernel
- Added mutex locking around shared elevator data
- Tested with producer and consumer and pushed files to GitHub

## Group Meetings

The group worked on the project together in informal sessions, with members
moving between parts to help each other.

| Attendees | Topics discussed |
|-----------|------------------|
| All members | Divided up the parts and reviewed the project requirements |
| All members | Kernel compilation and adding the system calls |
| All members | Elevator design: kthread, mutex and linked lists |
| All members | Testing the elevator and preparing the README and demo |

# COP4610-Project-2

# Project 2: Pet Elevator Kernel Module

COP4610: Operating Systems
Group 14: Jonathan Norton, Robyn Wilson, Arinze Abanah

## Division of Labor

Before the project (planned, submitted 10/9/26):

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

After the project:

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
├── README.md
├── .gitignore
├── part1/
│   ├── empty.c
│   ├── empty.trace
│   ├── part1.c
│   ├── part1.trace
│   └── Makefile
├── part2/
│   ├── src/
│   │   └── my_timer.c
│   └── Makefile
├── part3/
│   ├── src/
│   │   └── elevator.c
│   ├── syscalls.c
│   └── Makefile
```

- `part1/`: empty program and a program with exactly five added system calls,
  plus their strace output.
- `part2/`: `my_timer` kernel module that creates `/proc/timer`.
- `part3/`: `elevator` kernel module, the three system calls
  (`start_elevator`, `issue_request`, `stop_elevator`) and the proc entry
  `/proc/elevator`.

## How to Compile and Run

TODO

## Development Log

### Jonathan Norton

- Wrote the my_timer module and the /proc/timer read handler
- Added system calls 548, 549 and 550 to the kernel source
- Compiled and installed the custom kernel
- Implemented the elevator kthread loop and floor linked lists

### Robyn Wilson

- Worked on the my_timer module
- Implemented pet structs and per-floor linked lists
- Worked on the /proc/elevator output
- Formed README.md file

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

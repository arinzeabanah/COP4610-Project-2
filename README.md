# Elevator

COP4610 Project 2. Part 1 traces system calls with `strace`. Part 2 is a kernel module that
reports the time through `/proc/timer`. Part 3 adds three system calls to Linux 6.16.12 and
implements a pet elevator scheduler as a kernel module, with its state shown in `/proc/elevator`.

## Group Members
- **Jonathan Norton**: [FSU email]
- **Robyn Wilson**: [FSU email]
- **Arinze Abanah**: aca24f@fsu.edu

## Division of Labor

### Part 1: System Call Tracing
- **Responsibilities**: Write an empty C program and a copy with exactly five added system
  calls, then confirm the difference with `strace`.
- **Assigned to**: Robyn Wilson, Arinze Abanah

### Part 2: Timer Kernel Module
- **Responsibilities**: Write the `my_timer` module that creates `/proc/timer` and prints the
  current time and the time elapsed since the last read.
- **Assigned to**: Jonathan Norton, Robyn Wilson

### Part 3a: Adding System Calls
- **Responsibilities**: Add `start_elevator` (548), `issue_request` (549), and `stop_elevator`
  (550) to the kernel through function pointers the module sets.
- **Assigned to**: Jonathan Norton, Arinze Abanah

### Part 3b: Kernel Compilation
- **Responsibilities**: Configure, compile, and install Linux 6.16.12 with the new system calls.
- **Assigned to**: Jonathan Norton, Arinze Abanah

### Part 3c: Threads
- **Responsibilities**: Run the elevator in a kthread that loads, unloads, and moves.
- **Assigned to**: Jonathan Norton, Robyn Wilson

### Part 3d: Linked List
- **Responsibilities**: Store waiting pets per floor and pets on board in `linux/list.h` lists,
  with each pet allocated by `kmalloc`.
- **Assigned to**: Jonathan Norton, Robyn Wilson

### Part 3e: Mutexes
- **Responsibilities**: Protect all shared elevator and floor data with a mutex.
- **Assigned to**: Jonathan Norton, Arinze Abanah

### Part 3f: Scheduling Algorithm
- **Responsibilities**: Decide which way the elevator moves and enforce the capacity rules.
- **Assigned to**: Jonathan Norton, Robyn Wilson, Arinze Abanah

### Division of labor (after)
[Fill in what each member actually did. Update this before submitting.]

## File Listing
```
elevator/
├── Makefile                 Builds all three parts
├── README.md
├── .gitignore               Keeps executables and module build output out of Git
├── part1/
│   ├── empty.c              Empty program (baseline)
│   ├── empty.trace          strace output for empty
│   ├── part1.c              empty.c plus five system calls
│   ├── part1.trace          strace output for part1
│   └── Makefile
├── part2/
│   ├── src/
│   │   └── my_timer.c       /proc/timer module
│   └── Makefile             Produces my_timer.ko
└── part3/
    ├── src/
    │   ├── elevator.h       Shared constants, structs, and prototypes
    │   ├── main.c           Module load/unload, connects the system calls
    │   ├── requests.c       start_elevator, issue_request, stop_elevator handlers
    │   ├── scheduler.c      Elevator kthread and LOOK scheduling
    │   ├── proc.c           /proc/elevator output
    │   └── pets.c           Pet allocation, letters, and cleanup
    ├── tests/
    │   ├── syscheck/        Course test: checks the system calls exist
    │   └── elevator/        Course test: producer and consumer programs
    ├── syscalls.c           Copy of the kernel's syscalls/syscalls.c
    └── Makefile             Produces elevator.ko
```

# How to Compile & Execute

### Requirements
- **Compiler**: `gcc`
- **Kernel**: Linux 6.16.12 built with the system calls from `part3/syscalls.c` (Part 3 only).
  Parts 1 and 2 also work on the stock kernel.
- **Tools**: `make`, `strace`, kernel headers for the running kernel

Run `make` in the repository root to build all three parts, or `make` inside a part's folder to
build only that part. `make clean` removes build output.

## Part 1

### Compilation
```bash
cd part1
make
```
This builds the `empty` and `part1` executables.

### Execution
```bash
make trace
```
This runs both programs under `strace`, writes `empty.trace` and `part1.trace`, and prints their
line counts. `part1.trace` has five more lines than `empty.trace`.

## Part 2

### Compilation
```bash
cd part2
make
```
This builds `my_timer.ko` in `part2/`.

### Execution
```bash
sudo insmod my_timer.ko
cat /proc/timer
sleep 1
cat /proc/timer
sudo rmmod my_timer
```
The first read prints the current time. Each later read also prints the elapsed time since the
previous read.

## Part 3

### Kernel setup
1. Copy `part3/syscalls.c` to `syscalls/syscalls.c` in the kernel source, with a
   `syscalls/Makefile` containing `obj-y := syscalls.o`.
2. Add entries 548, 549, and 550 to `arch/x86/entry/syscalls/syscall_64.tbl`.
3. Declare the three system calls in `include/linux/syscalls.h`.
4. Set `core-y := syscalls/` in the kernel's top-level `Makefile`.
5. Compile, install, and reboot into the new kernel.

To confirm the system calls exist:
```bash
cd part3/tests/syscheck
make
sudo insmod syscheck.ko
make run
sudo rmmod syscheck
```

### Compilation
```bash
cd part3
make
```
This builds `elevator.ko` in `part3/`.

### Execution
```bash
sudo insmod elevator.ko
cd tests/elevator
make
./consumer --start
./producer 20
watch -n 1 cat /proc/elevator
./consumer --stop
sudo rmmod elevator
```

### /proc/elevator format
```
Elevator state: UP
Current floor: 2
Current load: 19 lbs
Elevator status: C4 D5

[ ] Floor 5: 0
[ ] Floor 4: 1 P1
[ ] Floor 3: 0
[*] Floor 2: 2 H5 C1
[ ] Floor 1: 0

Number of pets: 2
Number of pets waiting: 3
Number of pets serviced: 7
```
Each pet prints as its type letter and destination floor. C is a Chihuahua (3 lbs), P is a Pug
(14 lbs), H is a Pughuahua (10 lbs), and D is a Dachshund (16 lbs). `[*]` marks the elevator's
floor.

## Development Log
Each member records their contributions here.

### Arinze Abanah

| Date       | Work Completed / Notes |
|------------|------------------------|
| 2026-11-02 | Set up the VM. Wrote Part 1 and checked the five-call difference with strace. |
| 2026-11-02 | Wrote and tested the Part 2 timer module on kernel 5.4. |
| 2026-11-02 | Added system calls 548-550 to Linux 6.16.12, compiled and installed it. |
| 2026-11-02 | Wrote the Part 3 elevator module and set up the repository structure. |

### Jonathan Norton

| Date       | Work Completed / Notes |
|------------|------------------------|
| YYYY-MM-DD | [Description of task]  |

### Robyn Wilson

| Date       | Work Completed / Notes |
|------------|------------------------|
| YYYY-MM-DD | [Description of task]  |

## Meetings
Document in-person meetings, their purpose, and what was discussed.

| Date       | Attendees            | Topics Discussed | Outcomes / Decisions |
|------------|----------------------|------------------|-----------------------|
| YYYY-MM-DD | [Names]              | [Agenda items]   | [Actions/Next steps]  |

## Bugs
No known bugs.

## Considerations
- **Scheduling**: The elevator uses LOOK. It keeps moving in one direction while a pet on board
  or a waiting pet needs a floor in that direction, then turns around. It moves one floor at a
  time.
- **FIFO boarding**: Pets board from the front of their floor's queue. If the front pet would
  break the 5-pet or 50-lb limit, no one behind it boards until it does.
- **Locking**: One mutex protects the elevator, the floor queues, and the counters. The kthread
  releases it before every sleep, so system calls and `/proc/elevator` reads never wait for a
  1-2 second delay.
- **Requests while offline**: `issue_request` accepts valid pets while the elevator is offline.
  They wait on their floor until it starts.
- **Invalid requests**: Floors outside 1-5, a type outside 0-3, or matching start and
  destination floors return 1.
- **Stopping**: `stop_elevator` returns 1 if the elevator is offline or already stopping. While
  stopping, the elevator delivers the pets on board, picks up no one, then goes OFFLINE.
- **Restarting**: `start_elevator` resets the elevator to floor 1 with no load, as the
  assignment specifies. Pets still waiting from before are picked up.
- **Kernel paths**: In Linux 6.16 the system call table is at
  `arch/x86/entry/syscalls/syscall_64.tbl`, and the top-level Makefile line is `core-y :=`.

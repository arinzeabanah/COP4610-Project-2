// SPDX-License-Identifier: GPL-2.0
/*
 * syscalls.c: system calls 548-550 for the pet elevator.
 *
 * Each system call forwards to a function pointer. The elevator module
 * sets these pointers when it loads and clears them when it unloads.
 * Without the module loaded, each call returns -ENOSYS.
 */
#include <linux/kernel.h>
#include <linux/export.h>
#include <linux/syscalls.h>

int (*STUB_start_elevator)(void) = NULL;
int (*STUB_issue_request)(int, int, int) = NULL;
int (*STUB_stop_elevator)(void) = NULL;

EXPORT_SYMBOL(STUB_start_elevator);
EXPORT_SYMBOL(STUB_issue_request);
EXPORT_SYMBOL(STUB_stop_elevator);

SYSCALL_DEFINE0(start_elevator)
{
    int (*start_fn)(void) = READ_ONCE(STUB_start_elevator);

    return start_fn ? start_fn() : -ENOSYS;
}

SYSCALL_DEFINE3(issue_request, int, start_floor, int, destination_floor, int, type)
{
    int (*request_fn)(int, int, int) = READ_ONCE(STUB_issue_request);

    return request_fn ? request_fn(start_floor, destination_floor, type) : -ENOSYS;
}

SYSCALL_DEFINE0(stop_elevator)
{
    int (*stop_fn)(void) = READ_ONCE(STUB_stop_elevator);

    return stop_fn ? stop_fn() : -ENOSYS;
}

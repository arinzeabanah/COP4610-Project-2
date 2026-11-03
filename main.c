// SPDX-License-Identifier: GPL-2.0
/*
 * main.c: module load and unload for the pet elevator.
 *
 * Loading the module sets up the floors, creates /proc/elevator, starts the
 * elevator kthread, and connects system calls 548-550 to this module.
 * The elevator starts OFFLINE until start_elevator() is called.
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/err.h>
#include "elevator.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Group 14");
MODULE_DESCRIPTION("Pet elevator scheduler");

/* Defined in the kernel's syscalls/syscalls.c */
extern int (*STUB_start_elevator)(void);
extern int (*STUB_issue_request)(int, int, int);
extern int (*STUB_stop_elevator)(void);

struct pet_elevator pet_elevator;
struct building building;
DEFINE_MUTEX(elevator_lock);

static struct task_struct *elevator_thread;

static void elevator_reset(void)
{
	int i;

	pet_elevator.state = OFFLINE;
	pet_elevator.current_floor = MIN_FLOOR;
	pet_elevator.direction = 1;
	pet_elevator.load_weight = 0;
	pet_elevator.pet_count = 0;
	pet_elevator.stopping = false;
	INIT_LIST_HEAD(&pet_elevator.pets);

	for (i = 0; i < NUM_FLOORS; i++) {
		INIT_LIST_HEAD(&building.waiting[i]);
		building.waiting_count[i] = 0;
	}
	building.total_waiting = 0;
	building.serviced = 0;
}

static int __init pet_elevator_init(void)
{
	int ret;

	elevator_reset();

	ret = elevator_proc_create();
	if (ret)
		return ret;

	elevator_thread = kthread_run(elevator_run, NULL, "pet_elevator");
	if (IS_ERR(elevator_thread)) {
		elevator_proc_remove();
		return PTR_ERR(elevator_thread);
	}

	WRITE_ONCE(STUB_start_elevator, elevator_start);
	WRITE_ONCE(STUB_issue_request, elevator_issue_request);
	WRITE_ONCE(STUB_stop_elevator, elevator_stop);
	return 0;
}

static void __exit pet_elevator_exit(void)
{
	int i;

	/* Disconnect the system calls first so no new work arrives */
	WRITE_ONCE(STUB_start_elevator, NULL);
	WRITE_ONCE(STUB_issue_request, NULL);
	WRITE_ONCE(STUB_stop_elevator, NULL);

	kthread_stop(elevator_thread);
	elevator_proc_remove();

	/* Free any pets still on board or waiting */
	mutex_lock(&elevator_lock);
	pet_free_list(&pet_elevator.pets);
	for (i = 0; i < NUM_FLOORS; i++)
		pet_free_list(&building.waiting[i]);
	mutex_unlock(&elevator_lock);
}

module_init(pet_elevator_init);
module_exit(pet_elevator_exit);

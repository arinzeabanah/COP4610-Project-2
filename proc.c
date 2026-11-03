// SPDX-License-Identifier: GPL-2.0
/*
 * proc.c: the /proc/elevator file.
 *
 * Example output:
 *
 *   Elevator state: UP
 *   Current floor: 2
 *   Current load: 19 lbs
 *   Elevator status: C4 D5
 *
 *   [ ] Floor 5: 0
 *   [ ] Floor 4: 1 P1
 *   [ ] Floor 3: 0
 *   [*] Floor 2: 2 H5 C1
 *   [ ] Floor 1: 0
 *
 *   Number of pets: 2
 *   Number of pets waiting: 3
 *   Number of pets serviced: 7
 *
 * Each pet prints as its type letter and destination floor: C3 is a
 * Chihuahua going to floor 3. Letters: C Chihuahua, P Pug, H Pughuahua,
 * D Dachshund. [*] marks the floor the elevator is on.
 */
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include "elevator.h"

#define ENTRY_NAME	"elevator"
#define PERMS		0444
#define PARENT		NULL

static struct proc_dir_entry *elevator_entry;

static const char *state_name(enum elevator_state state)
{
	switch (state) {
	case OFFLINE:
		return "OFFLINE";
	case IDLE:
		return "IDLE";
	case LOADING:
		return "LOADING";
	case UP:
		return "UP";
	case DOWN:
		return "DOWN";
	}
	return "UNKNOWN";
}

static void print_pets(struct seq_file *m, struct list_head *pets)
{
	struct pet *pet;

	list_for_each_entry(pet, pets, list)
		seq_printf(m, " %c%d", pet_letter(pet->type), pet->dest_floor);
}

static int elevator_show(struct seq_file *m, void *v)
{
	int floor, index;

	mutex_lock(&elevator_lock);

	seq_printf(m, "Elevator state: %s\n", state_name(pet_elevator.state));
	seq_printf(m, "Current floor: %d\n", pet_elevator.current_floor);
	seq_printf(m, "Current load: %d lbs\n", pet_elevator.load_weight);
	seq_puts(m, "Elevator status:");
	print_pets(m, &pet_elevator.pets);
	seq_puts(m, "\n\n");

	for (floor = MAX_FLOOR; floor >= MIN_FLOOR; floor--) {
		index = floor - MIN_FLOOR;
		seq_printf(m, "[%c] Floor %d: %d",
			   floor == pet_elevator.current_floor ? '*' : ' ',
			   floor, building.waiting_count[index]);
		print_pets(m, &building.waiting[index]);
		seq_putc(m, '\n');
	}

	seq_printf(m, "\nNumber of pets: %d\n", pet_elevator.pet_count);
	seq_printf(m, "Number of pets waiting: %d\n", building.total_waiting);
	seq_printf(m, "Number of pets serviced: %d\n", building.serviced);

	mutex_unlock(&elevator_lock);
	return 0;
}

static int elevator_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, elevator_show, NULL);
}

static const struct proc_ops elevator_proc_ops = {
	.proc_open    = elevator_proc_open,
	.proc_read    = seq_read,
	.proc_lseek   = seq_lseek,
	.proc_release = single_release,
};

int elevator_proc_create(void)
{
	elevator_entry = proc_create(ENTRY_NAME, PERMS, PARENT, &elevator_proc_ops);
	return elevator_entry ? 0 : -ENOMEM;
}

void elevator_proc_remove(void)
{
	proc_remove(elevator_entry);
}

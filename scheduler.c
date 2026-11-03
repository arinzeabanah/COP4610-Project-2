// SPDX-License-Identifier: GPL-2.0
/*
 * scheduler.c: the elevator kthread and its scheduling algorithm.
 *
 * Each pass of the loop does one thing: load/unload at the current floor,
 * move one floor, go idle, or go offline. The lock is released before every
 * sleep, so system calls and proc reads never wait on a 1-2 second delay.
 *
 * Scheduling is LOOK: keep moving in the current direction while a pet on
 * board or a waiting pet needs a floor ahead, otherwise turn around. The
 * elevator moves one floor at a time, so it never jumps from 5 to 1.
 */
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/slab.h>
#include "elevator.h"

static int floor_index(int floor)
{
	return floor - MIN_FLOOR;
}

/* True if this pet fits within both the pet count and weight limits */
static bool pet_fits(const struct pet *pet)
{
	return pet_elevator.pet_count < MAX_PETS &&
	       pet_elevator.load_weight + pet->weight <= MAX_WEIGHT;
}

/* True if any pet on board is getting off at the current floor */
static bool needs_unload(void)
{
	struct pet *pet;

	list_for_each_entry(pet, &pet_elevator.pets, list) {
		if (pet->dest_floor == pet_elevator.current_floor)
			return true;
	}
	return false;
}

/* True if the pet at the front of this floor's queue can board (FIFO) */
static bool needs_load(void)
{
	struct list_head *queue;

	if (pet_elevator.stopping)
		return false;

	queue = &building.waiting[floor_index(pet_elevator.current_floor)];
	if (list_empty(queue))
		return false;

	return pet_fits(list_first_entry(queue, struct pet, list));
}

/* Lets off every pet whose destination is the current floor */
static void unload_pets(void)
{
	struct pet *pet, *next;

	list_for_each_entry_safe(pet, next, &pet_elevator.pets, list) {
		if (pet->dest_floor != pet_elevator.current_floor)
			continue;

		list_del(&pet->list);
		pet_elevator.pet_count--;
		pet_elevator.load_weight -= pet->weight;
		building.serviced++;
		kfree(pet);
	}
}

/*
 * Boards pets from the front of the queue until the next one doesn't fit.
 * Pets behind a pet that doesn't fit wait their turn, keeping FIFO order.
 */
static void load_pets(void)
{
	int index = floor_index(pet_elevator.current_floor);
	struct list_head *queue = &building.waiting[index];
	struct pet *pet;

	while (!pet_elevator.stopping && !list_empty(queue)) {
		pet = list_first_entry(queue, struct pet, list);
		if (!pet_fits(pet))
			break;

		list_move_tail(&pet->list, &pet_elevator.pets);
		pet_elevator.pet_count++;
		pet_elevator.load_weight += pet->weight;
		building.waiting_count[index]--;
		building.total_waiting--;
	}
}

/* Returns +1 to move up, -1 to move down, or 0 if there is nothing to do */
static int next_direction(void)
{
	int here = pet_elevator.current_floor;
	bool work_above = false;
	bool work_below = false;
	struct pet *pet;
	int floor;

	/* Pets on board pull the elevator toward their destinations */
	list_for_each_entry(pet, &pet_elevator.pets, list) {
		if (pet->dest_floor > here)
			work_above = true;
		else if (pet->dest_floor < here)
			work_below = true;
	}

	/* Waiting pets count too, unless the elevator is stopping */
	if (!pet_elevator.stopping) {
		for (floor = MIN_FLOOR; floor <= MAX_FLOOR; floor++) {
			if (building.waiting_count[floor_index(floor)] == 0)
				continue;
			if (floor > here)
				work_above = true;
			else if (floor < here)
				work_below = true;
		}
	}

	/* Keep going the same way while there is work ahead */
	if (pet_elevator.direction > 0 && work_above)
		return 1;
	if (pet_elevator.direction < 0 && work_below)
		return -1;

	/* Otherwise turn around if there is work behind */
	if (work_above)
		return 1;
	if (work_below)
		return -1;
	return 0;
}

/* The kthread body. Runs from module load until module unload. */
int elevator_run(void *data)
{
	int step;

	while (!kthread_should_stop()) {
		mutex_lock(&elevator_lock);

		if (pet_elevator.state == OFFLINE) {
			mutex_unlock(&elevator_lock);
			msleep(IDLE_POLL_MS);
			continue;
		}

		/* Load and unload at the current floor */
		if (needs_unload() || needs_load()) {
			pet_elevator.state = LOADING;
			mutex_unlock(&elevator_lock);

			ssleep(LOAD_DELAY_SEC);

			mutex_lock(&elevator_lock);
			unload_pets();
			load_pets();
			mutex_unlock(&elevator_lock);
			continue;
		}

		/* A stop finishes once the last pet on board is off */
		if (pet_elevator.stopping && pet_elevator.pet_count == 0) {
			pet_elevator.state = OFFLINE;
			pet_elevator.stopping = false;
			mutex_unlock(&elevator_lock);
			continue;
		}

		step = next_direction();
		if (step == 0) {
			pet_elevator.state = IDLE;
			mutex_unlock(&elevator_lock);
			msleep(IDLE_POLL_MS);
			continue;
		}

		/* Move one floor */
		pet_elevator.direction = step;
		pet_elevator.state = (step > 0) ? UP : DOWN;
		mutex_unlock(&elevator_lock);

		ssleep(MOVE_DELAY_SEC);

		mutex_lock(&elevator_lock);
		pet_elevator.current_floor += step;
		mutex_unlock(&elevator_lock);
	}

	return 0;
}

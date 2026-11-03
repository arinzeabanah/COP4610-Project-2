// SPDX-License-Identifier: GPL-2.0
/*
 * requests.c: the handlers behind system calls 548-550.
 * main.c points the kernel's STUB_ pointers at these functions.
 */
#include "elevator.h"

/*
 * start_elevator(): puts the elevator in service.
 * Returns 1 if it is already active (or still stopping), 0 on success.
 */
int elevator_start(void)
{
	int ret = 0;

	mutex_lock(&elevator_lock);

	if (pet_elevator.state != OFFLINE) {
		ret = 1;
	} else {
		pet_elevator.state = IDLE;
		pet_elevator.current_floor = MIN_FLOOR;
		pet_elevator.direction = 1;
		pet_elevator.load_weight = 0;
		pet_elevator.pet_count = 0;
		pet_elevator.stopping = false;
	}

	mutex_unlock(&elevator_lock);
	return ret;
}

/*
 * issue_request(): adds a pet to the back of its start floor's queue.
 * Returns 1 for an invalid request, -ENOMEM if allocation fails, 0 otherwise.
 * Pets may arrive while the elevator is offline; they wait until it starts.
 */
int elevator_issue_request(int start_floor, int dest_floor, int type)
{
	struct pet *pet;
	int index;

	if (start_floor < MIN_FLOOR || start_floor > MAX_FLOOR ||
	    dest_floor < MIN_FLOOR || dest_floor > MAX_FLOOR ||
	    start_floor == dest_floor ||
	    type < 0 || type >= NUM_PET_TYPES)
		return 1;

	pet = pet_create(type, start_floor, dest_floor);
	if (!pet)
		return -ENOMEM;

	index = start_floor - MIN_FLOOR;

	mutex_lock(&elevator_lock);
	list_add_tail(&pet->list, &building.waiting[index]);
	building.waiting_count[index]++;
	building.total_waiting++;
	mutex_unlock(&elevator_lock);

	return 0;
}

/*
 * stop_elevator(): stops picking up pets. The kthread finishes delivering
 * the pets on board, then sets the state to OFFLINE.
 * Returns 1 if the elevator is already stopping or offline, 0 otherwise.
 */
int elevator_stop(void)
{
	int ret = 0;

	mutex_lock(&elevator_lock);

	if (pet_elevator.state == OFFLINE || pet_elevator.stopping)
		ret = 1;
	else
		pet_elevator.stopping = true;

	mutex_unlock(&elevator_lock);
	return ret;
}

/* SPDX-License-Identifier: GPL-2.0 */
/*
 * elevator.h: shared definitions for the pet elevator module.
 *
 * All fields of pet_elevator and building are shared between the
 * system calls, the elevator kthread, and the proc file, so they are only
 * read or written while holding elevator_lock.
 */
#ifndef PET_ELEVATOR_H
#define PET_ELEVATOR_H

#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/types.h>

/* Building and elevator limits */
#define MIN_FLOOR	1
#define MAX_FLOOR	5
#define NUM_FLOORS	(MAX_FLOOR - MIN_FLOOR + 1)
#define MAX_PETS	5
#define MAX_WEIGHT	50	/* lbs */

/* Timing */
#define MOVE_DELAY_SEC	2	/* time to travel one floor */
#define LOAD_DELAY_SEC	1	/* time to load or unload at a floor */
#define IDLE_POLL_MS	100	/* how often an idle elevator checks for work */

/* Pet types, numbered as issue_request() receives them */
enum pet_type {
	CHIHUAHUA = 0,
	PUG = 1,
	PUGHUAHUA = 2,
	DACHSHUND = 3,
	NUM_PET_TYPES
};

enum elevator_state {
	OFFLINE,
	IDLE,
	LOADING,
	UP,
	DOWN
};

/* One pet, allocated with kmalloc when its request arrives */
struct pet {
	int type;
	int weight;
	int start_floor;
	int dest_floor;
	struct list_head list;	/* links the pet into a floor or the elevator */
};

struct pet_elevator {
	enum elevator_state state;
	int current_floor;
	int direction;		/* +1 when last moving up, -1 when moving down */
	int load_weight;	/* total lbs on board */
	int pet_count;		/* number of pets on board */
	bool stopping;		/* stop requested: unload everyone, load no one */
	struct list_head pets;	/* pets on board */
};

struct building {
	struct list_head waiting[NUM_FLOORS];	/* FIFO queue per floor, index 0 is floor 1 */
	int waiting_count[NUM_FLOORS];
	int total_waiting;
	int serviced;				/* pets delivered to their floor */
};

extern struct pet_elevator pet_elevator;
extern struct building building;
extern struct mutex elevator_lock;

/* pets.c */
struct pet *pet_create(int type, int start_floor, int dest_floor);
char pet_letter(int type);
void pet_free_list(struct list_head *head);

/* requests.c: the work behind system calls 548-550 */
int elevator_start(void);
int elevator_issue_request(int start_floor, int dest_floor, int type);
int elevator_stop(void);

/* scheduler.c */
int elevator_run(void *data);

/* proc.c */
int elevator_proc_create(void);
void elevator_proc_remove(void);

#endif /* PET_ELEVATOR_H */

// SPDX-License-Identifier: GPL-2.0
/*
 * pets.c: creating, describing, and freeing pets.
 */
#include <linux/slab.h>
#include "elevator.h"

/* Indexed by enum pet_type */
static const int pet_weights[NUM_PET_TYPES] = { 3, 14, 10, 16 };
static const char pet_letters[NUM_PET_TYPES] = { 'C', 'P', 'H', 'D' };

/* Allocates a pet. Returns NULL if memory runs out. The caller checks the type. */
struct pet *pet_create(int type, int start_floor, int dest_floor)
{
	struct pet *pet = kmalloc(sizeof(*pet), GFP_KERNEL);

	if (!pet)
		return NULL;

	pet->type = type;
	pet->weight = pet_weights[type];
	pet->start_floor = start_floor;
	pet->dest_floor = dest_floor;
	INIT_LIST_HEAD(&pet->list);
	return pet;
}

/* One-letter code used in /proc/elevator: C, P, H (Pughuahua), or D */
char pet_letter(int type)
{
	return pet_letters[type];
}

/* Removes and frees every pet in a list. The caller holds elevator_lock. */
void pet_free_list(struct list_head *head)
{
	struct pet *pet, *next;

	list_for_each_entry_safe(pet, next, head, list) {
		list_del(&pet->list);
		kfree(pet);
	}
}

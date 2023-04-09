#ifndef __ARENA_UTILS_H__
#define __ARENA_UTILS_H__
#include <inttypes.h>
#include <stddef.h>
#include "DoublyLinkedList.h"

typedef struct {
	uint64_t arena_size;
	doubly_linked_list_t *alloc_list;
} arena_t;

typedef struct {
	uint64_t start_address;
	size_t size;
	doubly_linked_list_t *miniblock_list;
} block_t;

typedef struct {
	uint64_t start_address;
	size_t size;
	uint8_t perm;
	void *rw_buffer;
} miniblock_t;

// Funcie care returneaza memoria libera din arena.
size_t free_memory(const arena_t *arena);

// Functie care returneaza numarul de miniblockuri din arena.
int number_miniblocks(const arena_t *arena);

#endif // __ARENA_UTILS_H__

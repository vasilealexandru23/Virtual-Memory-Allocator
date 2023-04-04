#pragma once
#include "DoublyLinkedList.h"
#include "utils.h"
#include <inttypes.h>
#include <stddef.h>

typedef struct {
	uint64_t start_address;
	size_t size;
	doubly_linked_list_t *miniblock_list; // capul listei de miniblockuri
} block_t;

typedef struct {
	uint64_t start_address;
	size_t size;
	uint8_t perm;	 // 6
	void *rw_buffer; // aloc is stochez doar la write
} miniblock_t;

typedef struct {
	uint64_t arena_size;
	doubly_linked_list_t *alloc_list;
} arena_t;

block_t create_block_t(const uint64_t address, const uint64_t size);
miniblock_t create_miniblock_t(const uint64_t address, const uint64_t size);

arena_t *alloc_arena(const uint64_t size);
void dealloc_arena(arena_t *arena);

int check_tangent_blocks_left(dll_node_t *curr_block, const uint64_t address);
int check_tangent_blocks_right(dll_node_t *curr_block, const uint64_t size,
							   const uint64_t address);
void remove_block_data(dll_node_t *curr_block);

void alloc_block(arena_t *arena, const uint64_t address, const uint64_t size);
void free_block(arena_t *arena, const uint64_t address);

int check_valid_zone(arena_t *arena, uint64_t address, uint64_t size,
					 int perm_bit);
void read(arena_t *arena, uint64_t address, uint64_t size);
void write(arena_t *arena, const uint64_t address, const uint64_t size,
		   int8_t *data);

size_t free_memory(const arena_t *arena);
int number_miniblocks(const arena_t *arena);

void pmap(const arena_t *arena);
void mprotect(arena_t *arena, uint64_t address, int8_t *permission);

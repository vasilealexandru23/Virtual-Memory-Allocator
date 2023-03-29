#pragma once
#include <inttypes.h>
#include <stddef.h>
#include "DoublyLinkedList.h"
#include "utils.h"
// #include "utilis.h"

typedef struct {
	uint64_t start_address;
	size_t size;
	list_t *miniblock_list; // capul listei de miniblockuri
} block_t;

typedef struct {
	uint64_t start_address;
	size_t size;
	uint8_t perm; // 6
	void *rw_buffer; // aloc is stochez doar la write
} miniblock_t;

typedef struct {
	uint64_t arena_size;
	list_t *alloc_list;
} arena_t;

arena_t *alloc_arena(const uint64_t size);
void dealloc_arena(arena_t *arena);

void alloc_block(arena_t *arena, const uint64_t address, const uint64_t size);
void free_block(arena_t *arena, const uint64_t address);

void read(arena_t *arena, uint64_t address, uint64_t size);
void write(arena_t *arena, const uint64_t address, const uint64_t size, int8_t *data);
void pmap(const arena_t *arena);
void mprotect(arena_t *arena, uint64_t address, int8_t *permission);

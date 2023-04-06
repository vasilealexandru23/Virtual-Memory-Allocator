#pragma once
#include "DoublyLinkedList.h"
#include "utils.h"
#include <inttypes.h>
#include <stddef.h>

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

typedef struct {
	uint64_t arena_size;
	doubly_linked_list_t *alloc_list;
} arena_t;

// Functie prin care returnez o noua structura de block.
block_t create_block_t(const uint64_t address, const uint64_t size);

// Functie prin care returnez o noua structura de miniblock.
miniblock_t create_miniblock_t(const uint64_t address, const uint64_t size);

// Functie prin care alloc memorie pentru structura
// de tip arena (si pe care o returnez).
arena_t *alloc_arena(const uint64_t size);

// Functie prin care eliberez toate resursele folosite.
void dealloc_arena(arena_t *arena);

// Functie prin care verific daca exista sau
// nu block tangent la stanga blocului curent.
int check_tangent_blocks_left(dll_node_t *curr_block, const uint64_t address);

// Functie prin care verific daca exista sau
// nu block tangent la dreapta blocului curent
int check_tangent_blocks_right(dll_node_t *curr_block, const uint64_t size,
							   const uint64_t address);

// Functie prin care eliberez un nod din lista de blockuri.
void remove_block_data(dll_node_t *curr_block);

// Functia de alocare a unui nou block.
void alloc_block(arena_t *arena, const uint64_t address, const uint64_t size);

// Functia de eliberare a memoriei a unui block.
void free_block(arena_t *arena, const uint64_t address);

// Functie prin care verific daca addresa de unde
// apelam read/write exista si are permisiunile aferente.
int check_valid_zone(arena_t *arena, uint64_t address, uint64_t size,
					 int perm_bit);

// Functie prin care afisam informatiile din
// bufferul zonei specificate de utilizator.
void read(arena_t *arena, uint64_t address, uint64_t size);

// Functie prin care introducem informatii
// in bufferul zonei specificate de utilizator.
void write(arena_t *arena, const uint64_t address, const uint64_t size,
		   int8_t *data);

// Funcie care returneaza memoria libera din arena.
size_t free_memory(const arena_t *arena);

// Functie care returneaza numarul de miniblockuri din arena.
int number_miniblocks(const arena_t *arena);

// Functia de afisare a informatiilor despre blockurile arenei.
void pmap(const arena_t *arena);

// Functie de atribuire de noi permisiuni a miniblockurilor.
void mprotect(arena_t *arena, uint64_t address, int8_t *permission);

#ifndef __BLOCK_UTILS_H__
#define __BLOCK_UTILS_H__
#include "arena_utils.h"
#define PERMISSION_SIZE 3

// Functie prin care returnez o noua structura de block.
block_t create_block_t(const uint64_t address, const uint64_t size);

// Functie prin care returnez o noua structura de miniblock.
miniblock_t create_miniblock_t(const uint64_t address, const uint64_t size);

// Functie prin care verific daca exista sau
// nu block tangent la stanga blocului curent.
int check_tangent_blocks_left(dll_node_t *curr_block, const uint64_t address);

// Functie prin care verific daca exista sau
// nu block tangent la dreapta blocului curent
int check_tangent_blocks_right(dll_node_t *curr_block, const uint64_t size,
							   const uint64_t address);

// Functie prin care eliberez un nod din lista de blockuri.
void remove_block_data(dll_node_t *curr_block);

// Functie prin care printam permisiunile unui miniblock.
void print_permissions(uint8_t permissions_value);

// Functie prin care verific daca addresa de unde
// apelam read/write exista si are permisiunile aferente.
int check_valid_zone(arena_t *arena, uint64_t address, uint64_t size,
					 int perm_bit);

#endif // __BLOCK_UTILS_H__

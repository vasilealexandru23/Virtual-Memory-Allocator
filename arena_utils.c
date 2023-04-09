#include "arena_utils.h"

size_t free_memory(const arena_t *arena)
{
	size_t memory = 0;
	dll_node_t *current_block = arena->alloc_list->head;

	// Parcurgem fiecare block si actualizam
	// contorul de memorie folosita din arena.
	while (current_block) {
		memory += ((block_t *)current_block->data)->size;
		current_block = current_block->next;
	}

	return arena->arena_size - memory;
}

int number_miniblocks(const arena_t *arena)
{
	int number = 0;
	dll_node_t *current_block = arena->alloc_list->head;

	// Parcurgem fiecare block si actualizam
	// numarul de miniblockuri gasite.
	while (current_block) {
		number +=
		    ((block_t *)current_block->data)->miniblock_list->size;
		current_block = current_block->next;
	}

	return number;
}

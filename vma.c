#include "vma.h"

arena_t *alloc_arena(const uint64_t size)
{
	arena_t *arena = (arena_t *)malloc(sizeof(arena_t));
    arena->arena_size = size;
	arena->alloc_list = dll_create(sizeof(block_t));
	return arena;
}

void dealloc_arena(arena_t *arena)
{
	node_t *current_block = arena->alloc_list->head;
	if (current_block == NULL) {
		dll_free(&arena->alloc_list);
		free(arena);
		arena = NULL;
		return;
	}
	// parcurg fiecare block, sterg informatiile din miniblockuri,
	// dau free la block si ma mut la nextul lui;
	while (current_block->next != NULL) {
		current_block = current_block->next;
		block_t *block_info = ((block_t *)current_block->prev->data);

		node_t *current_miniblock = block_info->miniblock_list->head;
		while (current_miniblock->next != NULL) {
			current_miniblock = current_miniblock->next;
			miniblock_t *miniblock_info = ((miniblock_t *)current_miniblock->prev->data);
			if (miniblock_info->rw_buffer)
				free(miniblock_info->rw_buffer);
			free(miniblock_info);
			free(current_miniblock->prev);
			current_miniblock->prev = NULL;
		}
		miniblock_t *miniblock_info = ((miniblock_t *)current_miniblock->data);
		if (miniblock_info->rw_buffer)
			free(miniblock_info->rw_buffer);
		free(miniblock_info);
		free(current_miniblock);
		current_miniblock = NULL;

		// dll_free(&block_info->miniblock_list);
		free(block_info);
		free(current_block->prev);
		current_block->prev = NULL;
	}
	// ultimul block

	block_t *block_info = ((block_t *)current_block->data);

	node_t *current_miniblock = block_info->miniblock_list->head;
	while (current_miniblock->next != NULL) {
		current_miniblock = current_miniblock->next;
		miniblock_t *miniblock_info = ((miniblock_t *)current_miniblock->prev->data);
		if (miniblock_info->rw_buffer)
			free(miniblock_info->rw_buffer);
		free(miniblock_info);
		free(current_miniblock->prev);
		current_miniblock->prev = NULL;
	}
	miniblock_t *miniblock_info = ((miniblock_t *)current_miniblock->data);
	if (miniblock_info->rw_buffer)
		free(miniblock_info->rw_buffer);
	free(miniblock_info);
	free(current_miniblock);
	current_miniblock = NULL;

	// dll_free(&block_info->miniblock_list);
	free(block_info);
	free(current_block);
	current_block = NULL;

	// dll_free(&arena->alloc_list);
	free(arena);
	arena = NULL;
}

void alloc_block(arena_t *arena, const uint64_t address, const uint64_t size)
{
	if (address >= arena->arena_size) {
		printf("The allocated address is outside the size of arena\n");
		return;
	} else if (address + size - 1 >= arena->arena_size) {
		printf("The end address is past the size of the arena\n");
		return;
	}
	// Parcurg fiecare block si îi verific daca memoria alocata 
	// se suprapune cu cea a blockului pe care vreau sa il inserez.
	node_t *current_block = arena->alloc_list->head;
	int index_block = 0;
	int index_block_prev = -1;
	int index_block_next = -1;
	node_t *prev_block = NULL;
	while (current_block != NULL) {
		block_t *block_info = ((block_t *)current_block->data);
		if (block_info->start_address + block_info->size - 1 >= address) {
			printf("This zone was already allocated.\n");
			return;
		}
		if (block_info->start_address + block_info->size == address) {
			index_block_prev = index_block;
			prev_block = current_block;
		}
		if (block_info->start_address == address + size)
			index_block_next = index_block;
		current_block = current_block->next;
		index_block++;
	}
	// Aloc memorie pentru un nou block.
	block_t *new_block = alloc_data(sizeof(block_t));
	new_block->start_address = address;
	new_block->size = size;
	// Aloc memorie pentru o noua lista de miniblockuri.
	new_block->miniblock_list = dll_create(sizeof(miniblock_t));
	miniblock_t *new_miniblock = alloc_data(sizeof(miniblock_t));
	new_miniblock->start_address = address;
	new_miniblock->size = size;
	new_miniblock->perm = 6;
	new_miniblock->rw_buffer = NULL;
	// Adaug noul miniblock in lista de miniblockuri din block - ul curent.
	dll_add_nth_node(new_block->miniblock_list, 0, new_miniblock);
	// check daca am blockuri tangente
	if (index_block_next != -1) { // am gasit block tangent urmator;
		node_t *removed = dll_remove_nth_node(arena->alloc_list, index_block_next);
		// concatenez miniblockurile	
		new_block->miniblock_list->head->next->next = ((block_t *)removed->data)->miniblock_list->head;
		((block_t *)removed->data)->miniblock_list->head->prev = new_block->miniblock_list->head->next;
		// maresc sizeurile
		new_block->miniblock_list->size += ((block_t *)removed->data)->size;
		new_block->size += ((block_t *)removed->data)->size; 
		free(((block_t *)removed->data)->miniblock_list);
		((block_t *)removed->data)->miniblock_list = NULL;
		free(removed->data);
		removed->data = NULL;
		// grija la freeuri
		dll_add_nth_node(arena->alloc_list, arena->alloc_list->size, new_block);
	} else {
		dll_add_nth_node(arena->alloc_list, arena->alloc_list->size, new_block);
	}

	if (index_block_prev != -1) { // am gasit block tangent precedent;
		// fac conexiunile
		node_t *removed = dll_remove_nth_node(arena->alloc_list, arena->alloc_list->size);
		block_t *prev_block_info = ((block_t *)prev_block->data);
		node_t *last_mini_block = dll_get_nth_node(prev_block_info->miniblock_list, prev_block_info->miniblock_list->size);
		last_mini_block->next = ((block_t *)removed->data)->miniblock_list->head;
		((block_t *)removed->data)->miniblock_list->head->prev = last_mini_block;
		// schimb sizeurile
		((block_t *)prev_block->data)->miniblock_list->size++;
		((block_t *)prev_block->data)->size++;
		// dau free
		free(prev_block_info->miniblock_list);
		prev_block_info->miniblock_list = NULL;
		free(prev_block_info);
		prev_block_info = NULL;
		free(removed);
		removed = NULL;
	}
	// AMIN
}

void free_block(arena_t *arena, const uint64_t address)
{
	// parcurg blockurile, verific daca adresa de unde vreau sa sterg
	// apartine intervalului address_block, address_block + size - 1;
	// daca da, atunci acela e blockul la care vreau sa sterg un anumit
	// miniblock, daca nu, continui sa parcurg blockurile.
	node_t *current_block = arena->alloc_list->head;
	while (current_block != NULL) {
		block_t *block_info = ((block_t *)current_block->data);
		if (address >= block_info->start_address && address <=
			block_info->start_address + block_info->size - 1) {
			// Incep sa parcurg miniblockurile.
			node_t *current_miniblock = block_info->miniblock_list->head;
			while (current_miniblock != NULL) {
				miniblock_t *miniblock_info = ((miniblock_t *)current_miniblock->data);
				if (miniblock_info->start_address == address) {
					// Am gasit miniblockul pe care vreau sa il sterg.
				}

				current_miniblock = current_miniblock->next;
			}
		}
	}
	// Nu am gasit adresa alocata.
	if (current_block == NULL)
		printf("Invalid address for free.\n");
}

void read(arena_t *arena, uint64_t address, uint64_t size)
{

}

void write(arena_t *arena, const uint64_t address, const uint64_t size, int8_t *data)
{

}

void pmap(const arena_t *arena)
{

}

void mprotect(arena_t *arena, uint64_t address, int8_t *permission)
{

}

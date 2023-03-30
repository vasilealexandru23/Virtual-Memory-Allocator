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
	dll_node_t *current_block = arena->alloc_list->head;
	if (current_block == NULL) {
		dll_free(&arena->alloc_list);
		free(arena);
		arena = NULL;
		return;
	}
	// parcurg fiecare block, sterg informatiile din miniblockuri,
	// dau free la block si ma mut la nextul lui;
	while (current_block != NULL) {
		block_t block_info = *((block_t *)current_block->data);

		dll_node_t *current_miniblock = block_info.miniblock_list->head;
		while (current_miniblock != NULL) {
			miniblock_t miniblock_info = *((miniblock_t *)current_miniblock->data);
			free(miniblock_info.rw_buffer);
			current_miniblock = current_miniblock->next;
		}
		dll_free(&block_info.miniblock_list);
		current_block = current_block->next;
	}

	dll_free(&arena->alloc_list);
	arena->alloc_list = NULL;
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
	// Parcurg fiecare block si \u00eei verific daca memoria alocata 
	// se suprapune cu cea a blockului pe care vreau sa il inserez.
	dll_node_t *current_block = arena->alloc_list->head;
	int index_block = 0;
	int index_block_prev = -1;
	int index_block_next = -1;
	dll_node_t *prev_block = NULL;
	while (current_block != NULL) {
		block_t block_info = *((block_t *)current_block->data);
		if (block_info.start_address + block_info.size - 1 >= address &&
			block_info.start_address <= address + size - 1) {
			printf("This zone was already allocated.\n");
			return;
		}
		if (block_info.start_address + block_info.size == address) {
			index_block_prev = index_block;
			prev_block = current_block;
		}
		if (block_info.start_address == address + size)
			index_block_next = index_block;
		current_block = current_block->next;
		index_block++;
	}
	// Aloc memorie pentru un nou block.
	block_t new_block;
	new_block.start_address = address;
	new_block.size = size;
	// Aloc memorie pentru o noua lista de miniblockuri.
	new_block.miniblock_list = dll_create(sizeof(miniblock_t));
	miniblock_t new_miniblock;
	new_miniblock.start_address = address;
	new_miniblock.size = size;
	new_miniblock.perm = 6;
	new_miniblock.rw_buffer = malloc(size);
	// Adaug noul miniblock in lista de miniblockuri din block - ul curent.
	dll_add_nth_node(new_block.miniblock_list, 0, &new_miniblock);
	// check daca am blockuri tangente
	if (index_block_next != -1) { // am gasit block tangent urmator;
		// printf("%d", index_block_next);
		dll_node_t *removed = dll_remove_nth_node(arena->alloc_list, index_block_next);
		/// concatenez miniblockurile
		new_block.miniblock_list->head->next = ((block_t *)removed->data)->miniblock_list->head;
		((block_t *)removed->data)->miniblock_list->head->prev = new_block.miniblock_list->head;
		// // maresc sizeurile
		new_block.miniblock_list->size += ((block_t *)removed->data)->size;
		new_block.size += ((block_t *)removed->data)->size; 
		free(((block_t *)removed->data)->miniblock_list);
		free(removed->data);
		free(removed);
		// grija la freeuri
		dll_add_nth_node(arena->alloc_list, arena->alloc_list->size, &new_block);
	} else {
		dll_add_nth_node(arena->alloc_list, arena->alloc_list->size, &new_block);
	}

	if (index_block_prev != -1) { // am gasit block tangent precedent;
	 	// fac conexiunile
	 	dll_node_t *removed = dll_remove_nth_node(arena->alloc_list, arena->alloc_list->size);
	 	block_t *prev_block_info = ((block_t *)prev_block->data);
	 	dll_node_t *last_mini_block = dll_get_nth_node(prev_block_info->miniblock_list, prev_block_info->miniblock_list->size);
	 	last_mini_block->next = ((block_t *)removed->data)->miniblock_list->head;
	 	((block_t *)removed->data)->miniblock_list->head->prev = last_mini_block;
	 	// schimb sizeurile
	 	((block_t *)prev_block->data)->miniblock_list->size++;
	 	((block_t *)prev_block->data)->size += ((block_t *)removed->data)->size;
	 	// dau free
		free(((block_t *)removed->data)->miniblock_list);
		free(removed->data);
		free(removed);
	}
	// AMIN
}

void free_block(arena_t *arena, const uint64_t address)
{
	// parcurg blockurile, verific daca adresa de unde vreau sa sterg
	// apartine intervalului address_block, address_block + size - 1;
	// daca da, atunci acela e blockul la care vreau sa sterg un anumit
	// miniblock, daca nu, continui sa parcurg blockurile.
	dll_node_t *current_block = arena->alloc_list->head;
	int index_block = 0;
	while (current_block != NULL) {
		block_t *block_info = ((block_t *)current_block->data);
		if (address >= block_info->start_address && address <=
			block_info->start_address + block_info->size - 1) {
			// Incep sa parcurg miniblockurile.
			dll_node_t *current_miniblock = block_info->miniblock_list->head;
			int index_miniblock = 0;
			while (current_miniblock != NULL) {
				miniblock_t *miniblock_info = ((miniblock_t *)current_miniblock->data);
				if (miniblock_info->start_address == address) {

					// Am gasit miniblockul pe care vreau sa il sterg.
					// Daca e la inceput, sau la final il sterg si dupa verific
					// daca mai exista alt nod;
					if (current_miniblock->prev == NULL || current_miniblock->next == NULL) {
						dll_node_t *removed = dll_remove_nth_node(block_info->miniblock_list, index_miniblock);
						block_info->size -= ((miniblock_t *)removed->data)->size;
						miniblock_t *removed_info = ((miniblock_t *)removed->data);
						if (removed_info->rw_buffer != NULL)
							free(removed->data);
						free(removed_info);
						free(removed);
						removed = NULL;
						// Verific daca mai exista miniblocuri in bloc.
						if (block_info->miniblock_list->size == 0) {
							dll_free(&block_info->miniblock_list);
							removed = dll_remove_nth_node(arena->alloc_list, index_block);
							free(removed);
							removed = NULL;
						}
					} else {
						// Trebuie sa creez alt block;
						current_miniblock->prev->next = NULL;
						block_info->miniblock_list->size = index_miniblock + 1;
						// Din sizeul total al unui miniblock, scad sizeul restul miniblocurilor;
						size_t size_miniblockuri_ramase = miniblock_info->size;
						int miniblocuri_ramase = 1;
						dll_node_t *aux = current_miniblock->next; 
						while (aux != NULL) {
							size_miniblockuri_ramase += ((miniblock_t *)aux->data)->size;
							miniblocuri_ramase++;
							aux = aux->next;
						}
						block_info->size -= size_miniblockuri_ramase;
						current_miniblock->prev = NULL; // Acum il fac capul liste de miniblocuri al unui nou bloc
						block_t *new_block = malloc(sizeof(block_t));
						new_block->start_address = miniblock_info->start_address;
						new_block->size = size_miniblockuri_ramase;
						new_block->miniblock_list = dll_create(sizeof(miniblock_t));
						new_block->miniblock_list->head = current_miniblock;
						new_block->miniblock_list->size = miniblocuri_ramase;
						dll_add_nth_node(arena->alloc_list, arena->alloc_list->size, new_block);
					}
					return;
				}

				current_miniblock = current_miniblock->next;
				index_miniblock++;
			}
		}
		index_block++;
		current_block = current_block->next;
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

size_t full_memory(const arena_t *arena)
{
	size_t memory = 0;
	dll_node_t *current_block = arena->alloc_list->head;
	while (current_block != NULL) {
		// printf("%d", memory);
		memory += ((block_t *)current_block->data)->size;
		current_block = current_block->next;
	}
	return memory;
}

int number_miniblocks(const arena_t *arena)
{
	int number = 0;
	dll_node_t *current_block = arena->alloc_list->head;
	while (current_block != NULL) {
		number += ((block_t *)current_block->data)->miniblock_list->size;
		current_block = current_block->next;
	}
	return number;
}

void pmap(const arena_t *arena)
{
	printf("Total memory : 0x%llX bytes\n", arena->arena_size);
	printf("Free memory : 0x%llX bytes\n", arena->arena_size - full_memory(arena));
	printf("Number of allocated blocks: %d\n", arena->alloc_list->size);
	printf("Number of allocated miniblocks : %d\n\n", number_miniblocks(arena));
	int index_block = 1;
	dll_node_t *current_block = arena->alloc_list->head;
	while (current_block != NULL) {
		block_t *block_info = ((block_t *)current_block->data);
		printf("Block %d begin\n", index_block);
		printf("Zone: 0x%llX - 0x%llX\n", block_info->start_address,
				block_info->start_address + block_info->size);
		dll_node_t *current_miniblock = block_info->miniblock_list->head;
		int miniblock_index = 1;
		while (current_miniblock != NULL) {
			miniblock_t *miniblock_info = ((miniblock_t *)current_miniblock->data);
			printf("Miniblock %d:\t\t0x%llX \t\t-\t\t0x%llX\t\t| %d\n", miniblock_index,
					miniblock_info->start_address, miniblock_info->start_address
					+ miniblock_info->size, miniblock_info->perm);
			miniblock_index++;
			current_miniblock = current_miniblock->next;
		}


		printf("Block %d end\n\n", index_block);
		index_block++;
		current_block = current_block->next;
	}
}

void mprotect(arena_t *arena, uint64_t address, int8_t *permission)
{

}

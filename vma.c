#include "vma.h"

block_t create_block_t(const uint64_t address, const uint64_t size)
{
	block_t new_block;
	new_block.start_address = address;
	new_block.size = size;
	// Aloc memorie pentru o noua lista de miniblockuri.
	new_block.miniblock_list = dll_create(sizeof(miniblock_t));
	return new_block;
}

miniblock_t create_miniblock_t(const uint64_t address, const uint64_t size)
{
	miniblock_t new_miniblock;
	new_miniblock.start_address = address;
	new_miniblock.size = size;
	new_miniblock.perm = 6;
	new_miniblock.rw_buffer = calloc(size, sizeof(char));
	return new_miniblock;
}

arena_t *alloc_arena(const uint64_t size)
{
	arena_t *arena = (arena_t *)malloc(sizeof(arena_t));
	arena->arena_size = size;
	arena->alloc_list = dll_create(sizeof(block_t));
	return arena;
}

void dealloc_arena(arena_t *arena)
{
	// Parcurgem fiecare nod din lista de blockuri.
	dll_node_t *curr_block = arena->alloc_list->head;

	while (curr_block) {
		block_t block_info = *((block_t *)curr_block->data);

		// Parcurgem fiecare nod din lista de miniblockuri.
		dll_node_t *curr_miniblock = block_info.miniblock_list->head;

		while (curr_miniblock) {
			miniblock_t miniblock_info =
			    *((miniblock_t *)curr_miniblock->data);

			// Eliberam memoria alocata bufferului
			// din miniblockul curent.
			free(miniblock_info.rw_buffer);

			curr_miniblock = curr_miniblock->next;
		}

		// Eliberam lista de noduri din blockul curent.
		dll_free(&block_info.miniblock_list);

		curr_block = curr_block->next;
	}

	// Eliberam lista de noduri din arena si arena in sine.
	dll_free(&arena->alloc_list);
	free(arena);
	arena = NULL;
}

int check_tangent_blocks_left(dll_node_t *curr_block, const uint64_t address)
{
	if (curr_block->prev &&
	    ((block_t *)curr_block->prev->data)->start_address +
		    ((block_t *)curr_block->prev->data)->size ==
		address)
		return 1;
	return 0;
}

int check_tangent_blocks_right(dll_node_t *curr_block, const uint64_t size,
							   const uint64_t address)
{
	if (curr_block &&
	    address + size == ((block_t *)curr_block->data)->start_address)
		return 1;
	return 0;
}

void remove_block_data(dll_node_t *curr_block)
{
	free(((block_t *)curr_block->data)->miniblock_list);
	free(curr_block->data);
	free(curr_block);
	curr_block = NULL;
}

void alloc_block(arena_t *arena, const uint64_t address, const uint64_t size)
{
	// Verificam daca parametrii dati de utilizatori sunt valizi.
	if (address >= arena->arena_size) {
		printf("The allocated address is outside the size of arena\n");
		return;
	} else if (address + size - 1 >= arena->arena_size) {
		printf("The end address is past the size of the arena\n");
		return;
	}

	// Gasim indexul unde trebuie introdus noul nod in lista de blockuri.
	int index_insert = 0;
	dll_node_t *current_block = arena->alloc_list->head;
	while (current_block) {
		block_t block_info = *((block_t *)current_block->data);
		if (block_info.start_address + block_info.size - 1 >= address &&
		    block_info.start_address <= address + size - 1) {
			printf("This zone was already allocated.\n");
			return;
		}
		if (address + size <= block_info.start_address)
			break;
		index_insert++;
		current_block = current_block->next;
	}

	// Aloc memorie pentru un nou block.
	block_t new_block = create_block_t(address, size);

	// Aloc memorie pentru o noua lista de miniblockuri.
	miniblock_t new_miniblock = create_miniblock_t(address, size);

	// Adaug noul miniblock in lista de miniblockuri din block - ul curent.
	dll_add_nth_node(new_block.miniblock_list, 0, &new_miniblock);

	// Verificam daca avem blockuri tangente la dreapta si stanga.
	if (check_tangent_blocks_right(current_block, size, address)) {
		// Stergem blockul din dreapta, din lista de
		// blockuri si unim miniblockurile.
		dll_node_t *removed =
		    dll_remove_nth_node(arena->alloc_list, index_insert);
		new_block.miniblock_list->head->next =
		    ((block_t *)removed->data)->miniblock_list->head;
		((block_t *)removed->data)->miniblock_list->head->prev =
		    new_block.miniblock_list->head;

		// Actualizam informatiile continute in blockul
		// curent si eliberam nodul sters.
		new_block.miniblock_list->size +=
		    ((block_t *)removed->data)->miniblock_list->size;
		new_block.size += ((block_t *)removed->data)->size;
		remove_block_data(removed);
	}

	dll_add_nth_node(arena->alloc_list, index_insert, &new_block);
	current_block = dll_get_nth_node(arena->alloc_list, index_insert);

	if (check_tangent_blocks_left(current_block, address)) {
		// Stergem nodul cu blockul adaugat anterior si unim miniblockurile.
		dll_node_t *removed =
		    dll_remove_nth_node(arena->alloc_list, index_insert);
		block_t prev_block_info =
		    *((block_t *)current_block->prev->data);
		dll_node_t *last_miniblock =
		    dll_get_nth_node(prev_block_info.miniblock_list,
							 prev_block_info.miniblock_list->size - 1);
		last_miniblock->next =
		    ((block_t *)removed->data)->miniblock_list->head;
		((block_t *)removed->data)->miniblock_list->head->prev =
		    last_miniblock;

		// Actualizam informatiile continute in blockul
		// tangent (la stanga) si eliberam nodul sters.
		((block_t *)current_block->prev->data)->miniblock_list->size +=
		    ((block_t *)removed->data)->miniblock_list->size;
		((block_t *)current_block->prev->data)->size +=
		    ((block_t *)removed->data)->size;
		remove_block_data(removed);
	}
}

void free_block(arena_t *arena, const uint64_t address)
{
	// Cautam blockul din care trebuie sters
	// miniblockul cu adresa data de utilizator.
	dll_node_t *current_block = arena->alloc_list->head;
	int index_block = 0;
	while (current_block) {
		block_t *block_info = ((block_t *)current_block->data);

		// Daca nu am gasit blockul care sa contina miniblockul
		// dat de utilizator, continui cautarea.
		if (!(address >= block_info->start_address &&
		      address < block_info->start_address + block_info->size)) {
		    index_block++;
			current_block = current_block->next;
			continue;
		}

		// Parcurg miniblockurile pana la cel ce trebuie sters.
		dll_node_t *current_miniblock = block_info->miniblock_list->head;
		int index_miniblock = 0;
		while (current_miniblock) {
			miniblock_t *miniblock_info =
				((miniblock_t *)current_miniblock->data);

			// Daca nu am gasit inca miniblockul, continui cautarea.
			if (miniblock_info->start_address != address) {
				current_miniblock =
					current_miniblock->next;
				index_miniblock++;
				continue;
			}

			// Miniblockul se afla fie la inceputul, fie la finalul
			// listei de miniblockuri din blockul curent.
			if (!current_miniblock->prev || !current_miniblock->next) {
				dll_node_t *removed =
					dll_remove_nth_node(block_info->miniblock_list,
										index_miniblock);
				miniblock_t *removed_info = ((miniblock_t *)removed->data);
				if (!removed->prev && removed->next)
					block_info->start_address =
					    ((miniblock_t *)removed->next->data)->start_address;
				block_info->size -= removed_info->size;
				free(removed_info->rw_buffer);
				free_node(removed);
				if (!block_info->miniblock_list->size) {
					dll_free(&block_info->miniblock_list);
					removed = dll_remove_nth_node(arena->alloc_list,
												  index_block);
					free_node(removed);
				}
			} else {
				current_miniblock->prev->next = NULL;
				block_info->miniblock_list->size = index_miniblock;
				size_t size_miniblockuri_ramase = miniblock_info->size;
				int miniblocuri_ramase = 1;
				dll_node_t *aux = current_miniblock->next;
				while (aux) {
					size_miniblockuri_ramase +=
					    ((miniblock_t *)aux->data)->size;
					miniblocuri_ramase++;
					aux = aux->next;
				}
				block_info->size -= size_miniblockuri_ramase;
				current_miniblock->prev = NULL;
				block_t new_block = create_block_t
				    (miniblock_info->start_address, size_miniblockuri_ramase);
				new_block.miniblock_list->head = current_miniblock;
				new_block.miniblock_list->size = miniblocuri_ramase;
				dll_add_nth_node(arena->alloc_list,
								 index_block + 1, &new_block);
				free_block(arena, miniblock_info->start_address);
			}
			return;
		}
		index_block++;
		current_block = current_block->next;
	}
	// Nu am gasit adresa blockului alocata.
	printf("Invalid address for free.\n");
}

int check_valid_zone(arena_t *arena, uint64_t address, uint64_t size,
					 int perm_bit)
{
	// Functie prin care verific daca addresa de unde
	// dau read/write exista si are permisiunile aferente.
	dll_node_t *current_block = arena->alloc_list->head;
	uint64_t chars = 0;
	int invalid_permision = 0;
	int to_print_overflow = 0;
	while (current_block) {
		block_t *block_info = ((block_t *)current_block->data);
		if (!(address >= block_info->start_address && address <=
			block_info->start_address + block_info->size - 1)) {
			current_block = current_block->next;
			continue;
		}
		if (block_info->start_address + block_info->size < address + size)
			to_print_overflow = 1;
		dll_node_t *curr_miniblock = block_info->miniblock_list->head;
		while (curr_miniblock) {
			miniblock_t *miniblock_info = ((miniblock_t *)curr_miniblock->data);
			unsigned long idx_buffer = 0;
			unsigned long upper_bound = address + size;
			while (idx_buffer <= miniblock_info->size - 1) {
				if (miniblock_info->start_address + idx_buffer >= address &&
					miniblock_info->start_address + idx_buffer < upper_bound) {
					if (((int)miniblock_info->perm) & (1 << perm_bit))
						chars++;
					else
						invalid_permision = 1;
				}
				idx_buffer++;
			}
			curr_miniblock = curr_miniblock->next;
		}
		break;
	}
	if (invalid_permision) {
		if (perm_bit == 2)
			printf("Invalid permissions for read.\n");
		else
			printf("Invalid permissions for write.\n");
		return 0;
	}

	if (!chars) {
		if (perm_bit == 2)
			printf("Invalid address for read.\n");
		else
			printf("Invalid address for write.\n");
		return 0;
	}

	if (to_print_overflow) {
		if (perm_bit == 2) {
			printf("Warning: size was bigger than the block size. Reading "
				   "%ld characters.\n", chars);
		} else {
			printf("Warning: size was bigger than the block size. Writing "
				   "%ld characters.\n", chars);
		}
	}
	return 1;
}

void read(arena_t *arena, uint64_t address, uint64_t size)
{
	if (!check_valid_zone(arena, address, size, 2))
		return;
	dll_node_t *current_block = arena->alloc_list->head;
	unsigned long found = 0;
	while (current_block) {
		block_t *block_info = ((block_t *)current_block->data);
		if (!(address >= block_info->start_address && address <=
			block_info->start_address + block_info->size - 1)) {
			current_block = current_block->next;
			continue;
		}
		// Sunt la blockul care trebuie
		// Incep sa parcurg miniblockurile.
		dll_node_t *curr_miniblock = block_info->miniblock_list->head;
		while (curr_miniblock) {
			miniblock_t *miniblock_info = ((miniblock_t *)curr_miniblock->data);
			unsigned long idx_buffer = 0;
			unsigned long upper_bound = address + size;
			int8_t *rw_buffer = (int8_t *)miniblock_info->rw_buffer;
			while (idx_buffer <= miniblock_info->size - 1) {
				if (miniblock_info->start_address + idx_buffer >= address &&
					miniblock_info->start_address + idx_buffer < upper_bound) {
					found++;
					if (rw_buffer[idx_buffer] != 0)
						printf("%c", rw_buffer[idx_buffer]);
				}
				idx_buffer++;
			}
			curr_miniblock = curr_miniblock->next;
		}
		if (found <= size)
			printf("\n");
		break;
	}
}

void write(arena_t *arena, const uint64_t address, const uint64_t size,
		   int8_t *data)
{
	if (!check_valid_zone(arena, address, size, 1))
		return;
	dll_node_t *current_block = arena->alloc_list->head;
	int index_data = 0;
	while (current_block) {
		block_t *block_info = ((block_t *)current_block->data);
		if (!(address >= block_info->start_address && address <=
			block_info->start_address + block_info->size - 1)) {
			current_block = current_block->next;
			continue;
		}
		// Sunt la blockul care trebuie
		// Incep sa parcurg miniblockurile.
		dll_node_t *curr_miniblock = block_info->miniblock_list->head;
		while (curr_miniblock) {
			miniblock_t *miniblock_info = ((miniblock_t *)curr_miniblock->data);
			unsigned long idx_buffer = 0;
			unsigned long upper_bound = address + size;
			int8_t *rw_buffer = (int8_t *)miniblock_info->rw_buffer;
			while (idx_buffer <= miniblock_info->size - 1) {
				if (miniblock_info->start_address + idx_buffer >= address &&
					miniblock_info->start_address + idx_buffer < upper_bound) {
					memcpy(rw_buffer + idx_buffer, data + index_data, 1);
					index_data++;
				}
				idx_buffer++;
			}
			curr_miniblock = curr_miniblock->next;
		}
		break;
	}
}

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

void pmap(const arena_t *arena)
{
	// Afisam formatul specificat in tema.
	printf("Total memory: 0x%lX bytes\n", arena->arena_size);
	printf("Free memory: 0x%lX bytes\n", free_memory(arena));
	printf("Number of allocated blocks: %d\n", arena->alloc_list->size);
	printf("Number of allocated miniblocks: %d\n",
	       number_miniblocks(arena));
	if (number_miniblocks(arena))
		printf("\n");

	// Parcurgem fiecare block din lista de blockuri.
	int index_block = 1;
	dll_node_t *current_block = arena->alloc_list->head;

	while (current_block) {
		block_t *block_info = ((block_t *)current_block->data);

		// Afisam informatiile despre blockul curent.
		printf("Block %d begin\n", index_block);
		printf("Zone: 0x%lX - 0x%lX\n", block_info->start_address,
		       block_info->start_address + block_info->size);

		// Parcurgem fiecare miniblock din lista de miniblockuri.
		dll_node_t *curr_miniblock = block_info->miniblock_list->head;
		int miniblock_index = 1;

		while (curr_miniblock) {
			miniblock_t *miniblock_info =
			    (miniblock_t *)curr_miniblock->data;

			// Afisam informatiile despre miniblockul curent.
			printf("Miniblock %d:\t\t0x%lX\t\t-\t\t0x%lX\t\t| ",
			       miniblock_index, miniblock_info->start_address,
			       miniblock_info->start_address +
				   miniblock_info->size);
			print_permissions(miniblock_info->perm);

			miniblock_index++;
			curr_miniblock = curr_miniblock->next;
		}

		current_block = current_block->next;

		printf("Block %d end\n", index_block);
		index_block++;
		if (current_block)
			printf("\n");
	}
}

void mprotect(arena_t *arena, uint64_t address, int8_t *permission)
{
	// Cautam blockurile care contin intervalul
	// de memorie specificat de utilizator.
	dll_node_t *curr_block = arena->alloc_list->head;
	while (curr_block) {
		block_t *block_info = ((block_t *)curr_block->data);

		// Blockul curent nu respecta intervalul
		// de memorie specificat de utilizator.
		if (!(address >= block_info->start_address &&
		      address <=
			  block_info->start_address + block_info->size - 1)) {
			curr_block = curr_block->next;
			continue;
		}

		// Cautam miniblockul specificat de utilizator.
		dll_node_t *curr_miniblock = block_info->miniblock_list->head;

		while (curr_miniblock) {
			miniblock_t *miniblock_info =
			    ((miniblock_t *)curr_miniblock->data);

			// Atribuim permisiunile miniblockului curent.
			if (miniblock_info->start_address == address) {
				miniblock_info->perm = *permission;
				return;
			}

			curr_miniblock = curr_miniblock->next;
		}

		curr_block = curr_block->next;
	}

	// Nu am gasit miniblockul cu
	// adresa specificata de utilizator.
	printf("Invalid address for mprotect.\n");
}

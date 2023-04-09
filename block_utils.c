#include "block_utils.h"

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

void print_permissions(uint8_t permissions_value)
{
	char *permissions = calloc((PERMISSION_SIZE + 1), sizeof(char));
	char permissions_options[] = "RWX";
	for (int i = 0; i < PERMISSION_SIZE; ++i)
		if (((int)permissions_value) & (1 << (PERMISSION_SIZE - 1 - i)))
			permissions[i] = permissions_options[i];
		else
			permissions[i] = '-';
	permissions[PERMISSION_SIZE] = '\0';
	printf("%s\n", permissions);
	free(permissions);
}

int check_valid_zone(arena_t *arena, uint64_t address, uint64_t size,
					 int perm_bit)
{
	dll_node_t *current_block = arena->alloc_list->head;
	uint64_t chars = 0;
	int invalid_permision = 0;
	int to_print_overflow = 0;

	// Parcurgem lista de blockuri pana
	// ajungem la adresa corespunzatoare.
	while (current_block) {
		block_t *block_info = ((block_t *)current_block->data);

		// Verific daca blockul curent este cel aferent.
		if (!(address >= block_info->start_address && address <=
			block_info->start_address + block_info->size - 1)) {
			current_block = current_block->next;
			continue;
		}

		// Verificam daca zona alocata rw_bufferului din blockul aferent
		// corespunde dimensiunii date de utilizator pentru read/write.
		if (block_info->start_address + block_info->size < address + size)
			to_print_overflow = 1;

		// Parcurgem lista de miniblockuri si verificam
		// permisiunile fiecaruia care se afla in intervalul aferent.
		dll_node_t *curr_miniblock = block_info->miniblock_list->head;
		while (curr_miniblock) {
			miniblock_t *miniblock_info = ((miniblock_t *)curr_miniblock->data);
			unsigned long idx_buffer = 0;
			unsigned long upper_bound = address + size;

			// Verificam permisiunile miniblockului curent.
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

	// Printam mesajele de eroare.

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

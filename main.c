#include <stdio.h>
#include "vma.h"

#define MAX_COMMAND_SIZE 64

int main(void) {

	char *command = (char *)malloc(MAX_COMMAND_SIZE * sizeof(char));
	arena_t *arena = NULL;
	while (1) {
		scanf("%s", command);

		if (strcmp(command, "ALLOC_ARENA") == 0) {
			uint64_t arena_size;
			scanf("%llu", &arena_size);
			arena = alloc_arena(arena_size);
		} else if (!strcmp(command, "DEALLOC_ARENA")) {
			dealloc_arena(arena);
			break;
		} else if (!strcmp(command, "ALLOC_BLOCK")) {
			uint64_t address, size;
			scanf("%llu %llu ", &address, &size);
			alloc_block(arena, address, size);
		} else if (!strcmp(command, "FREE_BLOCK")) {
			uint64_t address;
			scanf("%llu ", &address);
			free_block(arena, address);
		} else if (!strcmp(command, "READ")) {
			uint64_t address, size;
			scanf("%llu %llu ", &address, &size);
			read(arena, address, size);
		} else if (!strcmp(command, "WRITE")) {
			uint64_t address, size;
			int8_t data;
			scanf("%llu %llu %s ", &address, &size, &data);
			write(arena, address, size, &data);
		} else if (!strcmp(command, "PMAP")) {
			pmap(arena);
		} else if (!strcmp(command, "MPROTECT")) {
			uint64_t address;
			int8_t permission;
			scanf("%llu, %s ", &address, &permission);
			mprotect(arena, address, &permission);
		} else {
			printf("Invalid command. Please try again.\n");
		}

	}
	free(command);
	return 0;
}

#include "vma.h"
#include <stdio.h>

#define MAX_COMMAND_SIZE 64
#define DATA_LEN 10000

int main(void)
{
	char *command = (char *)malloc(MAX_COMMAND_SIZE * sizeof(char));
	arena_t *arena = NULL;
	while (1) {
		scanf("%s", command);

		if (strcmp(command, "ALLOC_ARENA") == 0) {
			uint64_t arena_size;
			scanf("%lu", &arena_size);
			arena = alloc_arena(arena_size);
		} else if (!strcmp(command, "DEALLOC_ARENA")) {
			dealloc_arena(arena);
			break;
		} else if (!strcmp(command, "ALLOC_BLOCK")) {
			uint64_t address, size;
			scanf("%lu%lu", &address, &size);
			alloc_block(arena, address, size);
		} else if (!strcmp(command, "FREE_BLOCK")) {
			uint64_t address;
			scanf("%lu ", &address);
			free_block(arena, address);
		} else if (!strcmp(command, "READ")) {
			uint64_t address, size;
			scanf("%lu%lu", &address, &size);
			read(arena, address, size);
		} else if (!strcmp(command, "WRITE")) {
			uint64_t address, size, error = 10000;
			scanf("%lu%lu", &address, &size);
			int8_t *data = malloc(size + error);
			getchar();
			char *full_text = calloc(size + error, sizeof(char));
			uint64_t cat_am_citit = 0;
			while (cat_am_citit < size) {
				char to_write[DATA_LEN];
				if (fgets(to_write, DATA_LEN, stdin)) {
					to_write[(int)strlen(to_write)] = '\0';
					strcat(full_text, to_write);
					cat_am_citit += strlen(to_write);
				}
			}
			full_text[size] = '\0';
			memcpy(data, full_text, size);
			write(arena, address, size, data);
			free(full_text);
			free(data);
		} else if (!strcmp(command, "PMAP")) {
			pmap(arena);
		} else if (!strcmp(command, "MPROTECT")) {
			uint64_t address;
			scanf("%lu ", &address);
			char new_permissions[DATA_LEN];
			fgets(new_permissions, DATA_LEN, stdin);
			new_permissions[(int)strlen(new_permissions)] = '\0';
			int8_t permission;
			int substract_permissions = 0;
			if (strstr(new_permissions, "PROT_NONE"))
				substract_permissions = 0;
			if (strstr(new_permissions, "PROT_READ"))
				substract_permissions += 4;
			if (strstr(new_permissions, "PROT_WRITE"))
				substract_permissions += 2;
			if (strstr(new_permissions, "PROT_EXEC"))
				substract_permissions += 1;
			permission = substract_permissions + '0';
			mprotect(arena, address, &permission);
		} else {
			printf("Invalid command. Please try again.\n");
		}
	}
	free(command);
	return 0;
}


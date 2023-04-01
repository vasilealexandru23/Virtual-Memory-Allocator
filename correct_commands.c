#include <stdio.h>
#include <string.h>
#define SEPARATORS "\n "
#define OPERATION_LEN 100

int check_number(char input[])
{
	// In aceasta functie verificam daca un sir de caracter
	// este un numar(pozitiv sau negativ);
	if (!input)
		return 0;
	if (!(input[0] == '-' || (input[0] >= '0' && input[0] <= '9')))
		return 0;
	for (int i = 1; i < (int)strlen(input); ++i)
		if (!(input[i] >= '0' && input[i] <= '9'))
			return 0;
	return 1;
}

int check_alloc_arena_cmd(char command[]) {
    char copy_command[OPERATION_LEN];
    strcpy(copy_command, command);
    copy_command[(int)strlen(command)] = '\0';
    char *input = strtok(copy_command, SEPARATORS);
    if (!input || strcmp(input, "ALLOC_ARENA"))
        return 0;
    input = strtok(NULL, SEPARATORS);
    if (!input || !check_number(input))
        return 0;
    input = strtok(NULL, SEPARATORS);
    if (input)
        return 0;
    return 1;
}

int check_dealloc_arena_cmd(char command[]) {
    char copy_command[OPERATION_LEN];
    strcpy(copy_command, command);
    copy_command[(int)strlen(command)] = '\0';
    char *input = strtok(copy_command, SEPARATORS);
    if (!input || strcmp(input, "DEALLOC_ARENA"))
        return 0;
    input = strtok(NULL, SEPARATORS);
    if (input)
        return 0;
    return 1;
}

int check_alloc_block_cmd(char command[]) {
    char copy_command[OPERATION_LEN];
    strcpy(copy_command, command);
    copy_command[(int)strlen(command)] = '\0';
    char *input = strtok(copy_command, SEPARATORS);
    if (!input || strcmp(input, "ALLOC_BLOCK"))
        return 0;
    input = strtok(NULL, SEPARATORS);
    if (!input || !check_number(input))
        return 0;
    input = strtok(NULL, SEPARATORS);
    if (!input || !check_number(input))
        return 0;
    input = strtok(NULL, SEPARATORS);
    if (input)
        return 0;
    return 1;
}

int check_free_block_cmd(char command[]) {
    char copy_command[OPERATION_LEN];
    strcpy(copy_command, command);
    copy_command[(int)strlen(command)] = '\0';
    char *input = strtok(copy_command, SEPARATORS);
    if (!input || strcmp(input, "FREE_BLOCK"))
        return 0;
    input = strtok(NULL, SEPARATORS);
    if (!input || !check_number(input))
        return 0;
    input = strtok(NULL, SEPARATORS);
    if (input)
        return 0;
    return 1;
}

int check_read_cmd(char command[]) {
    char copy_command[OPERATION_LEN];
    strcpy(copy_command, command);
    copy_command[(int)strlen(command)] = '\0';
    char *input = strtok(copy_command, SEPARATORS);
    if (!input || strcmp(input, "READ"))
        return 0;
    input = strtok(NULL, SEPARATORS);
    if (!input || !check_number(input))
        return 0;
    input = strtok(NULL, SEPARATORS);
    if (!input || !check_number(input))
        return 0;
    input = strtok(NULL, SEPARATORS);
    if (input)
        return 0;
    return 1;
}

int check_write_cmd(char command[]) {
    char copy_command[OPERATION_LEN];
    strcpy(copy_command, command);
    copy_command[(int)strlen(command)] = '\0';
    char *input = strtok(copy_command, SEPARATORS);
    if (!input || strcmp(input, "ALLOC_BLOCK"))
        return 0;
    input = strtok(NULL, SEPARATORS);
    if (!input || !check_number(input))
        return 0;
    input = strtok(NULL, SEPARATORS);
    if (!input || !check_number(input))
        return 0;
    input = strtok(NULL, SEPARATORS);
    if (!input || !check_number(input))
	    return 0;
    input = strtok(NULL, SEPARATORS);
    if (input)
        return 0;
    return 1;
}

int check_pmap_cmd(char command[]) {
    char copy_command[OPERATION_LEN];
    strcpy(copy_command, command);
    copy_command[(int)strlen(command)] = '\0';
    char *input = strtok(copy_command, SEPARATORS);
    if (strcmp(input, "PMAP"))
        return 0;
    input = strtok(NULL, SEPARATORS);
    if (input)
        return 0;
    return 1;
}

int check_mprotect_cmd(char command[]) {
    char copy_command[OPERATION_LEN];
    strcpy(copy_command, command);
    copy_command[(int)strlen(command)] = '\0';
    char *input = strtok(copy_command, SEPARATORS);
    if (strmp(input, "MPROTECT"))
        return 0;
    input = strtok(NULL, SEPARATORS);
    if (!input || !check_number(input))
	    return 0;
    input = strtok(NULL, SEPARATORS);
    return 1;
}

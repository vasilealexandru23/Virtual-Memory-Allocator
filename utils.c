#include "utils.h"

void *alloc_data(const uint64_t bytes)
{
	void *data = malloc(bytes);
	if (!data)
		printf("Eroare la alocare");
	return data;
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

#ifndef __UTILS__
#define __UTILS__

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stddef.h>
#define PERMISSION_SIZE 3

void *alloc_data(const uint64_t bytes);

void print_permissions(uint8_t permissions_value);

// block_t create_block_t(const uint64_t address, const uint64_t size);

#endif // __UTILS__
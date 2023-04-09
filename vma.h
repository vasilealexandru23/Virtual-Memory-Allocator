#pragma once
#include "DoublyLinkedList.h"
#include "block_utils.h"

// Functie prin care alloc memorie pentru structura
// de tip arena (si pe care o returnez).
arena_t *alloc_arena(const uint64_t size);

// Functie prin care eliberez toate resursele folosite.
void dealloc_arena(arena_t *arena);

// Functia de alocare a unui nou block.
void alloc_block(arena_t *arena, const uint64_t address, const uint64_t size);

// Functia de eliberare a memoriei a unui block.
void free_block(arena_t *arena, const uint64_t address);

// Functie prin care afisam informatiile din
// bufferul zonei specificate de utilizator.
void read(arena_t *arena, uint64_t address, uint64_t size);

// Functie prin care introducem informatii
// in bufferul zonei specificate de utilizator.
void write(arena_t *arena, const uint64_t address, const uint64_t size,
		   int8_t *data);

// Functia de afisare a informatiilor despre blockurile arenei.
void pmap(const arena_t *arena);

// Functie de atribuire de noi permisiuni a miniblockurilor.
void mprotect(arena_t *arena, uint64_t address, int8_t *permission);

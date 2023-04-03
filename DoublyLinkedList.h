#ifndef __DOUBLYLINKEDLIST__
#define __DOUBLYLINKEDLIST__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* useful macro for handling error codes */
#define DIE(assertion, call_description)				\
	do {												\
		if (assertion) {								\
			fprintf(stderr, "(%s, %d): ",				\
					__FILE__, __LINE__);				\
			perror(call_description);					\
			exit(errno);								\
		}												\
	} while (0)

typedef struct dll_node_t dll_node_t;
typedef struct dll_node_t {
	void *data;
	dll_node_t *prev, *next;
} dll_node_t;

typedef struct doubly_linked_list_t doubly_linked_list_t;
typedef struct doubly_linked_list_t {
	dll_node_t *head;
	unsigned int data_size;
	unsigned int size;
} doubly_linked_list_t;

doubly_linked_list_t *dll_create(unsigned int data_size);

dll_node_t *dll_get_nth_node(doubly_linked_list_t *list, unsigned int n);
void dll_add_nth_node(doubly_linked_list_t *list, unsigned int n,
					  const void *new_data);
dll_node_t *dll_remove_nth_node(doubly_linked_list_t *list, unsigned int n);

void dll_free(doubly_linked_list_t **pp_list);

#endif // __DOUBLYLINKEDLIST__

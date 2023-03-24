#ifndef __DOUBLYLINKEDLIST__
#define __DOUBLYLINKEDLIST__

#include <stdlib.h>
#include <string.h>

typedef struct node_t node_t;
struct node_t {
    void* data; 
    node_t *prev, *next;
};

typedef struct list_t list_t;
struct list_t {
    node_t* head;
    unsigned int data_size;
    unsigned int size;
}; 


list_t *dll_create(unsigned int data_size);

node_t* dll_get_nth_node(list_t* list, unsigned int n);
void dll_add_nth_node(list_t* list, unsigned int n, const void* new_data);
node_t* dll_remove_nth_node(list_t* list, unsigned int n);

void dll_free(list_t** pp_list);

#endif // __DOUBLYLINKEDLIST__
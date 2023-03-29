#include "DoublyLinkedList.h"

list_t *dll_create(unsigned int data_size) {

	list_t *list = malloc(sizeof(list_t));
	list->head = NULL;
	list->data_size = data_size;
	list->size = 0;

	return list;
}

node_t* dll_get_nth_node(list_t* list, unsigned int n) {
	n %= list->size;
	node_t *my_node = list->head;
	while (n--) {
		my_node = my_node->next;
	}
	return my_node;
}

void dll_add_nth_node(list_t* list, unsigned int n, const void* new_data) {
	if (n > list->size)
		n = list->size;

	node_t *new_node = malloc(sizeof(node_t));
	new_node->data = malloc(list->data_size);
	memcpy(new_node->data, new_data, list->data_size);

	if (n == 0 || list->size == 0) {
		// list is empty
		if (list->size == 0) {
			list->head = new_node;
            new_node->next = new_node;
            new_node->prev = new_node;
            list->size++;
            return;
		}
        // list has at least one element
		node_t *tail = (list->head)->prev;
        new_node->next = list->head;
        new_node->prev = tail;
        tail->next = new_node;
		(list->head)->prev = new_node;
        list->head = new_node;
        list->size++;
        return;
	}

    n--;
    node_t *curr = list->head;
    while (n--) {
        curr = curr->next;
    }

    new_node->next = curr->next;
    new_node->prev = curr;
    (curr->next)->prev = new_node;
    curr->next = new_node;
    list->size++;
}

node_t* dll_remove_nth_node(list_t* list, unsigned int n) {
	if (n > list->size - 1)
		n = list->size - 1;
	node_t *removed = list->head;
	if (n == 0) {
		list->head = list->head->next;
		list->head->prev = removed->prev;
		removed->prev->next = list->head;
		list->size--;
		return removed;
	}
	while (n--)
		removed = removed->next;
	node_t *prev = removed->prev;
	prev->next = removed->next;
	removed->next->prev = prev;
	list->size--;
	return removed;
}

void dll_free(list_t** pp_list) {
	node_t *curr = (*pp_list)->head;
	if (curr == NULL) {
		free((*pp_list));
		*pp_list = NULL;
		return;
	}
	while (curr->next != NULL) {
		curr = curr->next;
		free(curr->prev->data);
		free(curr->prev);
	}
    free(curr->data);
    free(curr);
	free((*pp_list));
	*pp_list = NULL;
}
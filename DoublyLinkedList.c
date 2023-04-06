#include "DoublyLinkedList.h"

doubly_linked_list_t *dll_create(unsigned int data_size)
{
	doubly_linked_list_t *list = malloc(sizeof(doubly_linked_list_t));
	DIE(!list, "Doubly_linked_list malloc failed !");
	list->head = NULL;
	list->data_size = data_size;
	list->size = 0;
	return list;
}

dll_node_t *dll_get_nth_node(doubly_linked_list_t *list, unsigned int n)
{
	if (!list || !list->size)
		return NULL;
	n %= list->size;
	dll_node_t *current = list->head;
	while (n--)
		current = current->next;
	return current;
}

void dll_add_nth_node(doubly_linked_list_t *list, unsigned int n,
					  const void *data)
{
	dll_node_t *new_node = malloc(sizeof(dll_node_t));
	DIE(!new_node, "New_node malloc failed !");

	new_node->data = malloc(list->data_size);
	DIE(!new_node->data, "New_node->data malloc failed !");

	memcpy(new_node->data, data, list->data_size);

	if (!n || !list->size) {
		new_node->prev = NULL;
		new_node->next = list->head;
		if (list->head)
			list->head->prev = new_node;
		list->head = new_node;
		list->size++;
		return;
	}

	if (n >= list->size) {
		new_node->next = NULL;
		dll_node_t *curr_node = list->head;
		while (curr_node->next)
			curr_node = curr_node->next;
		curr_node->next = new_node;
		new_node->prev = curr_node;
		list->size++;
		return;
	}

	dll_node_t *curr_node = list->head;
	while (--n)
		curr_node = curr_node->next;

	dll_node_t *next_node = curr_node->next;
	new_node->next = curr_node->next;
	new_node->prev = curr_node;
	curr_node->next = new_node;
	next_node->prev = new_node;
	list->size++;
}

dll_node_t *dll_remove_nth_node(doubly_linked_list_t *list, unsigned int n)
{
	if (list->size == 0)
		return NULL;

	if (n >= list->size - 1)
		n = list->size - 1;

	if (n == 0) {
		dll_node_t *to_remove = list->head;
		if (list->size > 1) {
			list->head = list->head->next;
			list->head->prev = NULL;
		} else {
			list->head = NULL;
		}
		list->size--;
		return to_remove;
	}

	dll_node_t *current = list->head;
	while (n--)
		current = current->next;

	dll_node_t *to_remove = current;

	if (!current->next) {
		dll_node_t *prev_node = current->prev;
		prev_node->next = NULL;
		list->size--;
		return to_remove;
	}

	dll_node_t *prev_node = current->prev;
	dll_node_t *next_node = current->next;
	prev_node->next = current->next;
	next_node->prev = current->prev;
	list->size--;
	return to_remove;
}

void free_node(dll_node_t *node)
{
	free(node->data);
	free(node);
}

void dll_free(doubly_linked_list_t **pp_list)
{
	if ((*pp_list)->size == 0) {
		free((*pp_list));
		return;
	}

	dll_node_t *prev_node = NULL;
	dll_node_t *curr_node = (*pp_list)->head;

	while (curr_node) {
		if (prev_node)
			free_node(prev_node);
		prev_node = curr_node;
		curr_node = curr_node->next;
	}

	if (prev_node)
		free_node(prev_node);

	free((*pp_list));
	*pp_list = NULL;
}

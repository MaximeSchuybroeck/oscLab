/**
 * \author Maxime Schuybroeck
 */

#ifndef _DPLIST_H_
#define _DPLIST_H_
/*
typedef enum {
    false, true
} bool; // or use C99
*/
#include <stdbool.h>
#include "../config.h"


/**
 * dplist_t is a struct containing at least a head pointer to the start of the list;
 */



typedef struct dplist dplist_t;

typedef struct dplist_node dplist_node_t;

struct dplist_node {
    dplist_node_t *prev, *next;
    element_t *element;
};

struct dplist {
    dplist_node_t *head;
};

/** Create and allocate memory for a new list
 * \return a pointer to a newly-allocated and initialized list.
 */
dplist_t *dpl_create();

/** Deletes all elements in the list
 * - Every list node of the list needs to be deleted. (free memory)
 * - The list itself also needs to be deleted. (free all memory)
 * - '*list' must be set to NULL.
 * \param list a double pointer to the list
 * \param free_element if true call element_free() on the element of the list node to remove
 */
void dpl_free(dplist_t **list, bool free_element);

/** Returns the number of elements in the list.
 * - If 'list' is is NULL, -1 is returned.
 * \param list a pointer to the list
 * \return the size of the list
 */
int dpl_size(dplist_t *list);

/** Inserts a new list node containing an 'element' in the list at position 'index'
 * - the first list node has index 0.
 * - If 'index' is 0 or negative, the list node is inserted at the start of 'list'.
 * - If 'index' is bigger than the number of elements in the list, the list node is inserted at the end of the list.
 * - If 'list' is is NULL, NULL is returned.
 * \param list a pointer to the list
 * \param element a pointer to the data that needs to be inserted
 * \param index the position at which the element should be inserted in the list
 * \param insert_copy if true use element_copy() to make a copy of 'element' and use the copy in the new list node, otherwise the given element pointer is added to the list
 * \return a pointer to the list or NULL
 */
dplist_t *dpl_insert_at_index(dplist_t *list, void *element, int index);

/** Removes the list node at index 'index' from the list.
 * - The list node itself should always be freed.
 * - If 'index' is 0 or negative, the first list node is removed.
 * - If 'index' is bigger than the number of elements in the list, the last list node is removed.
 * - If the list is empty, return the unmodified list.
 * - If 'list' is is NULL, NULL is returned.
 * \param list a pointer to the list
 * \param index the position at which the node should be removed from the list
 * \param free_element if true, call element_free() on the element of the list node to remove
 * \return a pointer to the list or NULL
 */
dplist_t *dpl_remove_at_index(dplist_t *list, int index, bool free_element);

/** Returns a reference to the list node with index 'index' in the list.
 * - If 'index' is 0 or negative, a reference to the first list node is returned.
 * - If 'index' is bigger than the number of list nodes in the list, a reference to the last list node is returned.
 * - If the list is empty, NULL is returned.
 * - If 'list' is is NULL, NULL is returned.
 * \param list a pointer to the list
 * \param index the position of the node for which the reference is returned
 * \return a pointer to the list node at the given index or NULL
 */
dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index);


#endif  // _DPLIST_H_


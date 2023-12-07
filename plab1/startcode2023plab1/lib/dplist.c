/**
*  \author Maxime Schuybroeck
*/

#include <stdio.h>
#include <stdlib.h>
#include "dplist.h"

struct dplist_node {
    dplist_node_t *prev, *next;
    void *element;
};

struct dplist {
    dplist_node_t *head;
    void *(*element_copy)(void *element);
    void (*element_free)(void **element);
    int (*element_compare)(void *x, void *y);
};

dplist_t *dpl_create(void *(*element_copy)(void *element), void (*element_free)(void **element), int (*element_compare)(void *x, void *y)) {
    dplist_t *list = malloc(sizeof(dplist_t));

    list->head = NULL;
    list->element_copy = element_copy;
    list->element_free = element_free;
    list->element_compare = element_compare;

    return list;
}

void dpl_free(dplist_t **list, bool free_element) {
    if (list == NULL || *list == NULL) {
        return;
    }

    while ((*list)->head != NULL) {
        dplist_node_t *temp = (*list)->head;
        (*list)->head = (*list)->head->next;

        if (free_element && (*list)->element_free != NULL) {
            (*list)->element_free(&(temp->element));
        }

        free(temp);
    }

    free(*list);
    *list = NULL;
}

int dpl_size(dplist_t *list) {
    if (list == NULL) {
        return -1;
    }

    int count = 0;
    dplist_node_t *current = list->head;

    while (current != NULL) {
        count++;
        current = current->next;
    }

    return count;
}

dplist_t *dpl_insert_at_index(dplist_t *list, void *element, int index, bool insert_copy) {
    // Implementation of inserting at a specific index
    // ...

    return list;
}

dplist_t *dpl_remove_at_index(dplist_t *list, int index, bool free_element) {
    // Implementation of removing at a specific index
    // ...

    return list;
}

dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index) {
    // Implementation of getting a reference at a specific index
    // ...

    return NULL;
}

void *dpl_get_element_at_index(dplist_t *list, int index) {
    // Implementation of getting an element at a specific index
    // ...

    return NULL;
}

int dpl_get_index_of_element(dplist_t *list, void *element) {
    // Implementation of getting the index of an element
    // ...

    return -1;
}

void *dpl_get_element_at_reference(dplist_t *list, dplist_node_t *reference) {
    // Implementation of getting an element at a specific reference
    // ...

    return NULL;
}

/**
 * \author Maxime Schuybroeck
 */

#include <stdlib.h>
#include "dplist.h"
#include <string.h>


dplist_t *dpl_create() {
    dplist_t *list = malloc(sizeof(struct dplist));
    list->head = NULL;
    return list;
}

void dpl_free(dplist_t **list, bool free_element) {
    int i = dpl_size(*list)-1;
    while(i>=0)
    {
        *list = dpl_remove_at_index(*list,i,free_element);
        i = i - 1;
    }
    free(*list);
    *list = NULL;
}

dplist_t *dpl_insert_at_index(dplist_t *list, void *givenElement, int index) {
    if (list == NULL) {
        return NULL;
    }

    dplist_node_t *new_node = malloc(sizeof(dplist_node_t));
    if (new_node == NULL) {
        return list;
    }
    new_node->element = givenElement;

    if (list->head == NULL || index <= 0) {
        new_node->prev = NULL;
        new_node->next = list->head;
        if (list->head != NULL) {
            list->head->prev = new_node;
        }
        list->head = new_node;
    } else {
        dplist_node_t *node_at_index = dpl_get_reference_at_index(list, index);
        new_node->next = node_at_index->next;
        new_node->prev = node_at_index;
        node_at_index->next = new_node;
    }

    return list;
}



dplist_t *dpl_remove_at_index(dplist_t *list, int index, bool free_element) {
    if(list == NULL){
        return NULL;
    } else if(list->head == NULL){
        return list;
    }
    if(index <= 0){
        dplist_node_t *current_node = (*list).head;
        (*list).head = current_node->next;
        if((*list).head != NULL){
            (*list).head->prev = NULL;
        }
        if(free_element && list->head != NULL){
            free(list->head->element);
        }
        free(current_node);
        return list;
    }
    dplist_node_t *node_to_delete = dpl_get_reference_at_index(list, index);
    if (node_to_delete == NULL) {
        return list;
    }
    if (index < dpl_size(list) - 1) {
        node_to_delete->prev->next = node_to_delete->next;
        node_to_delete->next->prev = node_to_delete->prev;
    } else {
        node_to_delete->prev->next = NULL;
    }
    if (free_element && node_to_delete->element != NULL) {
        free(node_to_delete->element);
    }
    free(node_to_delete);
    return list;
}

int dpl_size(dplist_t *list) {
    int size = 1;
    dplist_node_t *current_node = list->head;
    while(current_node->next != NULL){
        size++;
        current_node = current_node->next;
    }
    return size;
}

dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index) {
    if (list == NULL || list->head == NULL || index < 0) {
        return NULL;
    }
    if(index == 0){
        return list->head;
    }
    int count = 0;
    dplist_node_t *current_node = list->head;

    while (current_node->next != NULL && count < index) {
        current_node = current_node->next;
        count++;
    }

    return current_node;
}



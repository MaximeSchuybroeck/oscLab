/**
 * \author Maxime Schuybroeck
 */

#include <stdlib.h>
#include <stdio.h>
//TODO: wegdoen
#include <assert.h>
#include "dplist.h"
#include "../config.h"

#include <inttypes.h>
#include <string.h>



/*
dplist_t *dpl_create() {
    dplist_t *list = malloc(sizeof(struct dplist));
    list->head = NULL;
    return list;
}
//TODO: herstel 3
dplist_t *dpl_create(// callback functions
        void (*element_free)(void **element)
) {
    dplist_t *list = malloc(sizeof(struct dplist));
    list->head = NULL;
    //TODO: zien of dat dit niet weg moet
    list->element_free = element_free;
    return list;
}
*/
dplist_t *dpl_create(// callback functions
        void (*element_free)(void **element)
) {
    dplist_t *list = malloc(sizeof(struct dplist));
    list->head = NULL;
    //TODO: zien of dat dit niet weg moet
    list->element_free = element_free;
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
    /*
    if(*list == NULL){
        return;
    } else if((*list)->head == NULL){
        free(*list);
        *list = NULL;
    } else{
        for(int i = 0; i < dpl_size(*list); i++){
            *list = dpl_remove_at_index(*list,i,free_element);
        }
        free(*list);
    }
     */

}

dplist_t *dpl_insert_at_index(dplist_t *list, void *givenElement, int index, bool insert_copy) {
    if (list == NULL) {
        return NULL;
    }

    dplist_node_t *new_node = malloc(sizeof(dplist_node_t));
    if (new_node == NULL) {
        return list;
    }

    void *element = (insert_copy) ? list->element_copy(givenElement) : givenElement;
    new_node->element = element;

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
        /*
        if (node_at_index == NULL) {
            free(new_node);
            return list;
        }

        new_node->prev = node_at_index->prev;
        new_node->next = (index < dpl_size(list)) ? node_at_index : NULL;

        if (new_node->prev != NULL) {
            new_node->prev->next = new_node;
        } else {
            list->head = new_node;
        }

        if (new_node->next != NULL) {
            new_node->next->prev = new_node;
        }
        */
    }

    return list;
}


/*
dplist_t *dpl_insert_at_index(dplist_t *list, void *element, int index, bool insert_copy) {

    if (list == NULL) {
        return NULL;
    }
    dplist_node_t *new_node = malloc(sizeof(dplist_node_t));
    if (new_node == NULL) {
        return list; //checking if memory is allocated
    }
    if (insert_copy) {
        new_node->element = list->element_copy(element);
    } else {
        new_node->element = element;
    }
    new_node->next = NULL;
    new_node->prev = NULL;
    if (list->head == NULL) {
        // Insert at the beginning of the list
        new_node->next = list->head;
        if (list->head != NULL) {
            list->head->prev = new_node;
        }
        list->head = new_node;
    } else {
        // Insert at a specific index
        dplist_node_t *node_at_index = dpl_get_reference_at_index(list, index);
        new_node->prev = node_at_index->prev;
        new_node->next = node_at_index;
        node_at_index->prev->next = new_node;
        if (new_node->next != NULL) {
            new_node->next->prev = new_node;
        }
    }

    if(list->head == NULL ){
        list->head = new_node;
    } else if(index <= 0){
        list->head->prev = new_node;
        new_node->next = list->head;
        list->head = new_node;
    } else{
        dplist_node_t *node_at_index = dpl_get_reference_at_index(list,index);
        new_node->prev = node_at_index->prev;
        new_node->next = node_at_index;
        node_at_index->prev->next = new_node;
        if (new_node->next != NULL) {
            new_node->next->prev = new_node;
        }
    }

    return list;
}
*/
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
    /*
    if (list == NULL || list->head == NULL) {
        return 0;
    }
     */
    int size = 1;
    dplist_node_t *current_node = list->head;
    while(current_node->next != NULL){
        size++;
        current_node = current_node->next;
    }
    return size;
}

void *dpl_get_element_at_index(dplist_t *list, int index) {
    if (list == NULL || list->head == NULL) {
        return NULL;
    } else{
        return dpl_get_reference_at_index(list, index)->element;
    }
}

int dpl_get_index_of_element(dplist_t *list, void *element) {

    if (list == NULL || list->head == NULL || element == NULL) {
        return -1;
    }
    int index = 0;
    dplist_node_t *current_node = list->head;
    while(current_node->next != NULL){
        if (list->element_compare(current_node->element, element) == 0) {
            return index;
        }
        current_node = current_node->next;
        index++;
    }
    return -1;
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


/*
dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index) {

    if (list == NULL || list->head == NULL || index < 0) {
        return NULL;
    }
    int count = 0;
    dplist_node_t *current_node = list->head;
    while(current_node->next != NULL && count < index){
        current_node = current_node->next;
        count++;
    }
    return current_node;
}
*/
void *dpl_get_element_at_reference(dplist_t *list, dplist_node_t *reference) {

    if (list == NULL || list->head == NULL || reference == NULL) {
        return NULL;
    }
    dplist_node_t *current_node = list->head;
    while(current_node != reference){
        if(current_node->next == NULL){
            return NULL;
        }
        current_node = current_node->next;
    }
    return current_node->element;
}



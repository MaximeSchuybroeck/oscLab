/**
 * \author Maxime Schuybroeck
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../config.h"
#include "dplist.h"


/*
 * The real definition of struct list / struct node
 */
struct element {
    sensor_id_t *sensorId;
    room_id_t *roomId;
    sensor_value_t *average;
    double previousValues[RUN_AVG_LENGTH];
    sensor_ts_t ts;
};

struct dplist_node {
    dplist_node_t *prev, *next;
    element_t *element;
};

struct dplist {
    dplist_node_t *head;

    void *(*element_copy)(void *src_element);

    void (*element_free)(void **element);

    int (*element_compare)(void *x, void *y);
};


dplist_t *dpl_create(// callback functions
        void *(*element_copy)(void *src_element),
        void (*element_free)(void **element),
        int (*element_compare)(void *x, void *y)
) {
    dplist_t *list;
    list = malloc(sizeof(struct dplist));
    list->head = NULL;
    list->element_copy = element_copy;
    list->element_free = element_free;
    list->element_compare = element_compare;
    return list;
}

void dpl_free(dplist_t **list, bool free_element) {

    if(*list == NULL){
        return;
    } else if((*list)->head == NULL){
        free(*list);
        *list = NULL;
    } else{
        dplist_node_t *current_node = (*list)->head;
        dplist_node_t *node_to_free = current_node;
        while(current_node->next !=NULL ){
            node_to_free = current_node;
            current_node = current_node->next;
            if(free_element){
                (*list)->element_free(&(node_to_free->element));
            }
            free(node_to_free);
        }
        free(*list);
        *list = NULL;
    }

}

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
    if(list->head == NULL){
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
        if(free_element){
            (*list).element_free(&(current_node->element));
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
        list->element_free(&(node_to_delete->element));
    }
    free(node_to_delete);
    return list;
}

int dpl_size(dplist_t *list) {

    if (list == NULL || list->head == NULL) {
        return 0;
    }
    int size = 1;
    dplist_node_t *current_node = list->head;
    while(current_node->next != NULL){
        size++;
        current_node = current_node->next;
    }
    return size;
}

void *dpl_get_element_at_index(dplist_t *list, int index) {

    return dpl_get_reference_at_index(list, index)->element;
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
    int count = 0;
    dplist_node_t *current_node = list->head;
    while(current_node->next != NULL && count < index){
        current_node = current_node->next;
        count++;
    }
    return current_node;
}

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



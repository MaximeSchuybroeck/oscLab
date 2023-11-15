/**
 * \author Jeroen Van Aken, Bert Lagaisse, Ludo Bruynseels
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "dplist.h"



/*
 * The real definition of struct list / struct node
 */
struct dplist_node {
    dplist_node_t *prev, *next;
    element_t element;
};

struct dplist {
    dplist_node_t *head;
    // more fields will be added later
};

dplist_t *dpl_create() {
    dplist_t *list;
    list = malloc(sizeof(struct dplist));
    list->head = NULL;
  return list;
}

void dpl_free(dplist_t **list) {

    if(list == NULL || *list == NULL){
        return; //nothing to free
    }
    dplist_node_t *current_node = (*list)->head;
    while(current_node != NULL){
        dplist_node_t *node_to_free = current_node;
        current_node = current_node->next;
        free(node_to_free);
    }
    free(*list);
    *list = NULL;
    //Do extensive testing with valgrind.
}

/* Important note: to implement any list manipulation operator (insert, append, delete, sort, ...), always be aware of the following cases:
 * 1. empty list ==> avoid errors
 * 2. do operation at the start of the list ==> typically requires some special pointer manipulation
 * 3. do operation at the end of the list ==> typically requires some special pointer manipulation
 * 4. do operation in the middle of the list ==> default case with default pointer manipulation
 * ALWAYS check that you implementation works correctly in all these cases (check this on paper with list representation drawings!)
 **/


dplist_t *dpl_insert_at_index(dplist_t *list, element_t element, int index) {
    dplist_node_t *ref_at_index, *list_node;
    if (list == NULL) return NULL;

    list_node = malloc(sizeof(dplist_node_t));

    list_node->element = element;
    // pointer drawing breakpoint
    if (list->head == NULL) { // covers case 1
        list_node->prev = NULL;
        list_node->next = NULL;
        list->head = list_node;
        // pointer drawing breakpoint
    } else if (index <= 0) { // covers case 2
        list_node->prev = NULL;
        list_node->next = list->head;
        list->head->prev = list_node;
        list->head = list_node;
        // pointer drawing breakpoint
    } else {
        ref_at_index = dpl_get_reference_at_index(list, index);
        assert(ref_at_index != NULL);
        // pointer drawing breakpoint
        if (index < dpl_size(list)) { // covers case 4
            list_node->prev = ref_at_index->prev;
            list_node->next = ref_at_index;
            ref_at_index->prev->next = list_node;
            ref_at_index->prev = list_node;
            // pointer drawing breakpoint
        } else { // covers case 3
            assert(ref_at_index->next == NULL);
            list_node->next = NULL;
            list_node->prev = ref_at_index;
            ref_at_index->next = list_node;
            // pointer drawing breakpoint
        }
    }
    return list;
}

dplist_t *dpl_remove_at_index(dplist_t *list, int index) {

    dplist_node_t *note_to_delete = dpl_get_reference_at_index(list, index);
    if(note_to_delete == NULL){
        return list;
    }
    if(note_to_delete ==list->head){
        //removing the head note
        list->head = note_to_delete->next;
        if(list->head != NULL){
            list->head->prev = NULL;
        }
    } else if(note_to_delete->next == NULL){
        //removing the last node
        note_to_delete->prev->next = NULL;
    } else{
        //removing a node somewhere in the middle
        note_to_delete->prev->next = note_to_delete->next;
        note_to_delete->next->prev = note_to_delete->prev;
    }

    free(note_to_delete); // freeing the memory of the deleted node
    return NULL;
}

int dpl_size(dplist_t *list) {

    if(list->head == NULL){
        return 0;
    } else{
        int size = 1;
        dplist_node_t *current_node = list->head;
        while (current_node->next != NULL){
            size++;
        }
        return size;
    }
}

dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index) {
    if(list ==  NULL || list->head == NULL || index < 0){
        return NULL;
    } else if(index == 0){
        return list->head;
    } else{
        if(dpl_size(list) < index) {
            index = dpl_size(list);
        }
        int count = 0 ;
        dplist_node_t *answer = list->head;
        while(count < index && answer != NULL){
            answer = answer->next;
            count++;
        }
        return answer;
    }
}

element_t dpl_get_element_at_index(dplist_t *list, int index) {

    dplist_node_t *wanted_node = dpl_get_reference_at_index(list, index);
    return wanted_node->element;
}

int dpl_get_index_of_element(dplist_t *list, element_t element) {

    if(list == NULL || list->head == NULL || element < 0){
        return -1;
    } else{
        dplist_node_t *current_note = list->head;
        int index = 0;
        while (current_note != NULL){
            if(current_note->element == element){
                return index;
            }
            index++;
            current_note = current_note->next;
        }
        return -1;
    }
}




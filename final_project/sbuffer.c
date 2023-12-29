/**
 * \author Maxime Schuybroeck
 */

#include "config.h"
#include <stdlib.h>
#include "sbuffer.h"
#include <pthread.h>
#include <stdbool.h>


pthread_mutex_t thread_mutex;
pthread_cond_t wait_condition = PTHREAD_COND_INITIALIZER;

typedef struct sbuffer_node {
    struct sbuffer_node *next;  /**< a pointer to the next node*/
    sensor_data_t data;         /**< a structure containing the data */
} sbuffer_node_t;


struct sbuffer {
    sbuffer_node_t *head;       /**< a pointer to the first node in the buffer */
    sbuffer_node_t *tail;       /**< a pointer to the last node in the buffer */
};

int sbuffer_init(sbuffer_t **buffer) {
    *buffer = malloc(sizeof(sbuffer_t));
    if (*buffer == NULL) return SBUFFER_FAILURE;
    (*buffer)->head = NULL;
    (*buffer)->tail = NULL;
    pthread_cond_init(&wait_condition, NULL);
    return SBUFFER_SUCCESS;
}

int sbuffer_free(sbuffer_t **buffer) {
    sbuffer_node_t *dummy;
    if ((buffer == NULL) || (*buffer == NULL)) {
        return SBUFFER_FAILURE;
    }
    while ((*buffer)->head) {
        dummy = (*buffer)->head;
        (*buffer)->head = (*buffer)->head->next;
        free(dummy);
    }
    pthread_mutex_destroy(&thread_mutex);
    pthread_cond_destroy(&wait_condition);
    free(*buffer);
    *buffer = NULL;
    return SBUFFER_SUCCESS;
}

int sbuffer_remove(sbuffer_t *buffer, sensor_data_t *data) {
    sbuffer_node_t *dummy;
    if (buffer == NULL) return SBUFFER_FAILURE;
    pthread_mutex_lock(&thread_mutex);
    while (buffer->head == NULL){
        pthread_cond_wait(&wait_condition, &thread_mutex);
    }
    if (buffer->head == NULL) {
        pthread_mutex_unlock(&thread_mutex);
        return SBUFFER_NO_DATA;
    }
    if(!buffer->head->data.read_by_datamgr){
        pthread_mutex_unlock(&thread_mutex);
        return SBUFFER_NOT_YET_READ;
    }
    *data = buffer->head->data;
    dummy = buffer->head;
    if (buffer->head == buffer->tail) // buffer has only one node
    {
        buffer->head = buffer->tail = NULL;
    } else  // buffer has many nodes empty
    {
        buffer->head = buffer->head->next;
    }
    free(dummy);
    pthread_mutex_unlock(&thread_mutex);
    return SBUFFER_SUCCESS;
}

int sbuffer_read(sbuffer_t *buffer, sensor_data_t *data) {
    if (buffer == NULL) return SBUFFER_FAILURE;
    pthread_mutex_lock(&thread_mutex);

    // waiting for the connection_tread to finish the first insert
    while (buffer->head == NULL){
        pthread_cond_wait(&wait_condition, &thread_mutex);
    }
    bool found_one = false;
    sbuffer_node_t *current_node = buffer->head;
    while(current_node != NULL){
        if(!current_node->data.read_by_datamgr){
            current_node->data.read_by_datamgr = true;
            *data = current_node->data;
            found_one = true;
            break;
        } else{
            current_node = current_node->next;
        }
    }
    if(!found_one){
        pthread_mutex_unlock(&thread_mutex);
        return SBUFFER_NOT_YET_READ;
    }
    pthread_mutex_unlock(&thread_mutex);
    return SBUFFER_SUCCESS;
}



int sbuffer_insert(sbuffer_t *buffer, sensor_data_t *data) {
    sbuffer_node_t *dummy;
    if (buffer == NULL) return SBUFFER_FAILURE;
    dummy = malloc(sizeof(sbuffer_node_t));
    if (dummy == NULL) return SBUFFER_FAILURE;
    pthread_mutex_lock(&thread_mutex);
    dummy->data = *data;
    dummy->next = NULL;
    if (buffer->tail == NULL) // buffer empty (buffer->head should also be NULL
    {
        buffer->head = buffer->tail = dummy;
    } else // buffer not empty
    {
        buffer->tail->next = dummy;
        buffer->tail = buffer->tail->next;
    }
    pthread_mutex_unlock(&thread_mutex);
    pthread_cond_signal(&wait_condition);
    return SBUFFER_SUCCESS;
}
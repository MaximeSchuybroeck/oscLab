/**
 * \author Maxime Schuybroeck
 */
#include "lib/dplist.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>
#include "datamgr.h"
#include "sbuffer.h"
#include <string.h>


// global variable
extern sbuffer_t *buffer;
int8_t index_value = 0;
dplist_t *list;


void * element_copy(void *element){
    element_t *copy = (element_t *) malloc(sizeof(element_t));
    if (copy == NULL) {
        exit(EXIT_FAILURE);
    }
    memcpy(copy, element, sizeof(element_t));
    return (void *)copy;
}

void element_free(void **element){
    free(*element);
    *element = NULL;
}

int element_compare(void *X, void *Y){
    //checking if they are NULL

    element_t *x = (element_t *) X;
    element_t *y = (element_t *) Y;
    if(x->sensorId == 0 || y->sensorId == 0){
        return -1;
    }

    if(x->sensorId == y->sensorId){
        return 0;
    }else return -1;
}

void datamgr_parse_room_sensor_map(FILE *fp_sensor_map) {
    // checking if the file pointers are NULL
    if (fp_sensor_map == NULL) {
        fprintf(stderr, "Error because the file points to NULL\n");
        return;
    }

    int room_id, sensor_id;
    int insert_index = 0;
    list = dpl_create(element_free);
    //list = (dplist_t *) malloc(sizeof(dplist_t));
    //dummy_node->element = NULL;
    //list->head = dummy_node;
    //list->head = dummy_node;

    // reading from the sensor map file
    while (fscanf(fp_sensor_map, "%d %d", &room_id, &sensor_id) == 2) {
        // iserting scanned data in a new element
        element_t *new_element = (element_t *)malloc(sizeof(element_t));
        new_element->roomId = room_id;
        new_element->sensorId = sensor_id;

        // add the new element to the list
        list = dpl_insert_at_index(list, new_element, insert_index, false);
        //list = dpl_insert_at_index(list, new_element, dpl_size(list) -1, false);
        insert_index++;
        //TODO: printf weg doen
        printf("Room ID: %d, Sensor ID: %d\n", room_id, sensor_id);
    }
}



void add_sensor_value(sensor_data_t *valueList[RUN_AVG_LENGTH], sensor_data_t *value){
    valueList[index_value] = value;
    index_value++;
    if(index_value > 4){
        index_value = 0;
    }
}

sensor_value_t calculate_avg(double valueList[RUN_AVG_LENGTH]){
    double total = 0.0;
    for (int i = 0; i < RUN_AVG_LENGTH; i++){
        total = total + valueList[i];
    }
    return total/RUN_AVG_LENGTH;
}


void datamgr_free(){
    dpl_free(&list, true);
}

room_id_t datamgr_get_room_id(sensor_id_t sensor_id){
    // checking if given sensor_id is valid
    if(sensor_id <= 0){
        ///TODO: ERROR_HANDLER nog oplossen
        //ERROR_HANDLER("sensor_id is invalid");
        return -1;
    }

    dplist_node_t *current_node = list->head;
    while (current_node->next != NULL){
        if(current_node->element->sensorId == sensor_id){
            return current_node->element->roomId;
        }
        current_node = current_node->next;
    }
    return -1;
}

sensor_value_t datamgr_get_avg(sensor_id_t sensor_id){
    // checking if given sensor_id is valid
    if(sensor_id <= 0){
        ///TODO: ERROR_HANDLER nog oplossen
        //ERROR_HANDLER("sensor_id is invalid");
        return -1.0;
    }

    dplist_node_t *current_node = list->head;
    while (current_node->next != NULL){
        if(current_node->element->sensorId == sensor_id){
            // calculating average
            sensor_value_t total = 0;
            int number_of_values = 0;
            for(int i = 0; i < 5; i++){
                if(current_node->element->previousValues[i] > 0){
                    total = total + current_node->element->previousValues[i];
                    number_of_values++;
                }
            }
            return total/number_of_values;
        }
        current_node = current_node->next;
    }
    return -1.0;
}

time_t datamgr_get_last_modified(sensor_id_t sensor_id){
    // checking if given sensor_id is valid
    if(sensor_id <= 0){
        ///TODO: ERROR_HANDLER nog oplossen
        //ERROR_HANDLER("sensor_id is invalid");
        return time(NULL);
    }

    dplist_node_t *current_node = list->head;
    while (current_node->next != NULL){
        if(current_node->element->sensorId == sensor_id){
            return current_node->element->ts;
        }
        current_node = current_node->next;
    }
    return time(NULL);
}

int datamgr_get_total_sensors(){
    dplist_node_t *current_node = list->head;
    sensor_id_t sensor_counter[dpl_size(list)]; // same size as the dplist since the sensor list by definition can only be smaller
    int index_value = 0;

    // initializing the array to avoid garbage values
    for (int i = 0; i < dpl_size(list); i++) {
        sensor_counter[i] = 0;
    }

    while (current_node != NULL){
        // checking if the sensor is already present in the list
        bool already_present = false;
        for (int i = 0; i < index_value; i++) {
            if (sensor_counter[i] == current_node->element->sensorId) {
                already_present = true;
                break;
            }
        }
        if(!already_present){
            sensor_counter[index_value] = current_node->element->sensorId;
            index_value++;
        }
        current_node = current_node->next;
    }
    return index_value + 1;
}

void *data_manager_thread() {
    // reading data from buffer via the sbuffer_read() function
    bool node_was_found;
    sensor_data_t *data = (sensor_data_t *) malloc(sizeof(sensor_data_t));
    while(1){
        if(sbuffer_read(buffer, data) != SBUFFER_SUCCESS){
            fprintf(stderr, "Error, in reading the buffer\n");
            break; // while loop stops if end or error is hit
        }
        if(data->id < 1){
            char msg[55];
            snprintf(msg, sizeof(msg), "Received sensor data with invalid sensor node ID %" PRIu16 "\n", data->id);
            write_to_log_process(msg);
        }

        // updating the dplist
        dplist_node_t *current_node = list->head;
        node_was_found = false;

        while(current_node != NULL){
            if(current_node->element->sensorId == data->id){
                current_node->element->ts = data->ts;
                //adding value to the list with previous values
                current_node->element->previousValues[index_value] = (data->value);
                index_value++;
                if(index_value > 4){
                    index_value = 0;
                }

                node_was_found = true;
                // writing the log message
                char msg[55];
                if(data->value > datamgr_get_avg(current_node->element->sensorId)){
                    snprintf(msg, sizeof(msg), "Sensor node %" PRIu16 " reports it’s too warm\n", data->id);

                }else{
                    snprintf(msg, sizeof(msg), "Sensor node %" PRIu16 " reports it’s too cold\n", data->id);
                }
                write_to_log_process(msg);
                break;
            }
            current_node = current_node->next;
        }

        // checking if the node was found
        if(!node_was_found){
            fprintf(stderr, "Error, sensor was not found in de dplist\n");
        }


    }
    free(data);
    return NULL;

}

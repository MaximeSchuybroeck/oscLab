/**
 * \author Maxime Schuybroeck
 */
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>
#include "./lib/dplist.h"
#include "datamgr.h"
#include "sbuffer.h"

//TODO: dplist.c nog weg doen
#include "lib/dplist.c"

// locale variable
extern sbuffer_t *buffer;
int8_t index = 0;


static dplist_t *list;



//TODO: deze code nog bezien wa ik er mee doe, van hier
static void * element_copy(void *element){
    return element;
}

static void element_free(void **element){
    free(*element);
    *element = NULL;
}

static int element_compare(void *X, void *Y){
    return 0;
}
// tot hier

void datamgr_parse_room_sensor_map(FILE *fp_sensor_map){
    // checking if the file pointers are NULL
    if (fp_sensor_map == NULL) {
        fprintf(stderr, "Error because the file points to NULL\n");
        return;
    }

    int room_id, sensor_id;
    list = dpl_create(element_copy, element_free, element_compare);

    // reading from the sensor map file
    dplist_node_t *current_node = list->head;
    while(fscanf(fp_sensor_map, "%d %d", &room_id, &sensor_id) == 2){
        current_node->element->roomId = room_id;
        current_node->element->sensorId = sensor_id;
        current_node = current_node->next;
        printf("Room ID: %d, Sensor ID: %d\n", room_id, sensor_id);
    }
}

void add_sensor_value(sensor_data_t *valueList[RUN_AVG_LENGTH], sensor_data_t *value){
    valueList[index] = value;
    index++;
    if(index > 4){
        index = 0;
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
    int index = 0;

    // initializing the array to avoid garbage values
    for (int i = 0; i < dpl_size(list); i++) {
        sensor_counter[i] = 0;
    }

    while (current_node != NULL){
        // checking if the sensor is already present in the list
        bool already_present = false;
        for (int i = 0; i < index; i++) {
            if (sensor_counter[i] == current_node->element->sensorId) {
                already_present = true;
                break;
            }
        }
        if(!already_present){
            sensor_counter[index] = current_node->element->sensorId;
            index++;
        }
        current_node = current_node->next;
    }
    return index + 1;
}

void *data_manager_thread() {
    // reading data from buffer via the sbuffer_read() function
    bool node_was_found;

    while(1){
        sensor_data_t data;     // to save the data in
        if(sbuffer_read(buffer, &data) != SBUFFER_SUCCESS){
            fprintf(stderr, "Error, in reading the buffer\n");
            break; // while loop stops if end or error is hit
        }

        // updating the dplist
        dplist_node_t *current_node = list->head;
        node_was_found = false;

        while(current_node != NULL){
            if(current_node->element->sensorId == data.id){
                current_node->element->ts = data.ts;
                //adding value to the list with previous values
                current_node->element->previousValues[index] = (data.value);
                index++;
                if(index > 4){
                    index = 0;
                }

                node_was_found = true;
                break;
            }
            current_node = current_node->next;
        }

        // checking if the node was found
        if(!node_was_found){
            fprintf(stderr, "Error, sensor was not found in de dplist\n");
        }
    }
    return NULL;

}

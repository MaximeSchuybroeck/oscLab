/**
 * \author Maxime Schuybroeck
 */
#include "lib/dplist.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "datamgr.h"
#include "sbuffer.h"
#include <string.h>


// global variable
extern sbuffer_t *buffer;
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

int datamgr_parse_room_sensor_map() {
    FILE *fp_sensor_map = fopen("room_sensor.map", "r");
    // checking if the file pointers are NULL
    if (fp_sensor_map == NULL) {
        //TODO END: printf
        //fprintf(stderr, "Error because the file points to NULL\n");
        printf("Error because the file points to NULL\n");
        return -1;
    }

    int room_id, sensor_id;
    int insert_index = 0;
    //TODO: herstel 2
    list = dpl_create(element_free);
    //list = dpl_create();

    // reading from the sensor map file
    while (fscanf(fp_sensor_map, "%d %d", &room_id, &sensor_id) == 2) {
        // iserting scanned data in a new element
        element_t *new_element = malloc(sizeof(element_t));
        new_element->roomId = room_id;
        new_element->sensorId = sensor_id;

        for(int i = 0; i < RUN_AVG_LENGTH; i++){
            new_element->previousValues[i] = 0;
        }

        // add the new element to the list
        list = dpl_insert_at_index(list, new_element, insert_index, false);
        //TODO: mischien insert_copy wel op true zetten --> zie 2todos hieronder
        insert_index++;
        //TODO END: printf weg doen
        printf("Room ID: %d, Sensor ID: %d\n", room_id, sensor_id);
    }
    //TODO: nog element free afh van insert_copy hierboven --> als true dan moet ik freeen anders niet
    //free(new_element)
    fclose(fp_sensor_map);
    return 0;
}

sensor_value_t calculate_avg(sensor_value_t valueList[RUN_AVG_LENGTH]){
    sensor_value_t total = 0.0;
    sensor_value_t index = 0.0;
    for (int i = 0; i < RUN_AVG_LENGTH; i++){
        if(valueList[i] == 0){
            break;
        }
        total = total + valueList[i];
        index = index + 1.0;
    }
    return total/index;
}


int get_valuelist_size(sensor_value_t valueList[RUN_AVG_LENGTH]){
    int size = 0;
    for(int i = 0; i < RUN_AVG_LENGTH; i++){
        if(valueList[i] == 0){
            break;
        }else{
            size++;
        }
    }
    return size;
}


void datamgr_free(){
    dpl_free(&list, true);
}

void *data_manager_thread() {
    // reading data from buffer via the sbuffer_read() function
    bool node_was_found;
    sensor_data_t *data = (sensor_data_t *) malloc(sizeof(sensor_data_t));
    while(1){
        int result = sbuffer_read(buffer, data);
        if(result != SBUFFER_NOT_YET_READ){
            if(result != SBUFFER_SUCCESS){
                //TODO END: printf
                //fprintf(stderr, " Datamgr Error, in reading the buffer\n");
                printf(" Datamgr Error, in reading the buffer\n");
                break; // while loop stops if end or error is hit
            }

            //TODO: zien wa ik hiermee doe
            if(data->id < 1){
                char msg[55];
                snprintf(msg, sizeof(msg), "Received end-of-stream marker with sensor ID %" PRIu16 "\n", data->id);
                write_to_log_process(msg);
                break;
            }

            // updating the dplist
            dplist_node_t *current_node = list->head;
            node_was_found = false;

            while(current_node != NULL){
                if(current_node->element->sensorId == data->id){
                    current_node->element->ts = data->ts;
                    //adding value to the list with previous values
                    current_node->element->previousValues[get_valuelist_size(current_node->element->previousValues)] = data->value;

                    node_was_found = true;
                    // writing the log message
                    char msg[55];
                    if(data->value > calculate_avg(current_node->element->previousValues)){
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
                //TODO:
                //fprintf(stderr, "Error, sensor %" PRIu16 " was not found in de dplist\n", data->id);
                printf("Error, sensor %" PRIu16 " was not found in de dplist\n", data->id);
            }
        }
    }
    free(data);
    //TODO END
    printf("!!!!!!!!!!!!!! DATA EINDE\n");
    return NULL;

}



/*
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
*/

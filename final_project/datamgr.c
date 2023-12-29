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
    list = dpl_create();

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
        list = dpl_insert_at_index(list, new_element, insert_index);
        insert_index++;
        //TODO END: printf weg doen
        printf("Room ID: %d, Sensor ID: %d\n", room_id, sensor_id);
    }
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
                    char msg[256];
                    sensor_value_t avg = calculate_avg(current_node->element->previousValues);
                    if(data->value > avg){
                        snprintf(msg, sizeof(msg), "Sensor node %" PRIu16 " reports it’s too warm (avg temp = %g)\n", data->id, avg);

                    }else{
                        snprintf(msg, sizeof(msg), "Sensor node %" PRIu16 " reports it’s too cold (avg temp = %g)\n", data->id, avg);
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
    return NULL;
}
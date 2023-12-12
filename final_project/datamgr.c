/**
 * \author Maxime Schuybroeck
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "config.h"
#include "lib/tcpsock.h"
#include "lib/dplist.h"
#include "lib/dplist.c"
#include <pthread.h>
#include "datamgr.h"
#include <stdbool.h>

// locale variable
dplist_t list;


void datamgr_parse_room_sensor_map(FILE *fp_sensor_map){
    // checking if the file pointers are NULL
    if (fp_sensor_map == NULL) {
        fprintf(stderr, "Error because the file points to NULL\n");
        return;
    }

    int room_id, sensor_id;

    // reading from the sensor map file
    dplist_node_t *current_node = list.head;
    while(fscanf(fp_sensor_map, "%d %d", &room_id, &sensor_id) == 2){
        current_node.element->roomId = room_id;
        current_node.element->sensorId = sensor_id;
        current_node = current_node.next;
        printf("Room ID: %d, Sensor ID: %d\n", room_id, sensor_id);
    }
}

dplist_t get_dplist(){
    return &list;
}

void datamgr_free(){
    dpl_free(&list, true);
}

uint16_t datamgr_get_room_id(sensor_id_t sensor_id){
    // checking if given sensor_id is valid
    if(sensor_id <= 0){
        ERROR_HANDLER("sensor_id is invalid");
        return -1;
    }

    dplist_node_t *current_node = list.head;
    while (current_node.next != NULL){
        if(current_node.element->sensorId == sensor_id){
            return current_node.element->roomId;
        }
        current_node = current_node.next;
    }
    return -1;
}

sensor_value_t datamgr_get_avg(sensor_id_t sensor_id){
    // checking if given sensor_id is valid
    if(sensor_id <= 0){
        ERROR_HANDLER("sensor_id is invalid");
        return -1;
    }

    dplist_node_t *current_node = list.head;
    while (current_node.next != NULL){
        if(current_node.element->sensorId == sensor_id){
            return current_node.element->average;
        }
        current_node = current_node.next;
    }
    return -1;
}

time_t datamgr_get_last_modified(sensor_id_t sensor_id){
    // checking if given sensor_id is valid
    if(sensor_id <= 0){
        ERROR_HANDLER("sensor_id is invalid");
        return -1;
    }

    dplist_node_t *current_node = list.head;
    while (current_node.next != NULL){
        if(current_node.element->sensorId == sensor_id){
            return current_node.element->ts;
        }
        current_node = current_node.next;
    }
    return -1;
}

int datamgr_get_total_sensors(){
    dplist_node_t *current_node = list.head;
    sensor_id_t sensor_counter[dpl_size(&list)]; // same size as the dplist since the sensor list by definition can only be smaller
    int index = 0;

    // initializing the array to avoid garbage values
    for (int i = 0; i < dpl_size(&list); i++) {
        sensor_counter[i] = 0;
    }

    bool already_present = false;
    while (current_node != NULL){
        // checking if the sensor is already present in the list
        already_present = false;
        for (int i = 0; i < dpl_size(&list); i++) {
            if (sensor_counter[i] == current_node->element->sensorId) {
                already_present = true;
                break;
            }
        }
        if(!already_present){
            sensor_counter[index] = current_node.element->sensorId;
            index++;
        }
        current_node = current_node.next;
    }
    return index + 1;
}


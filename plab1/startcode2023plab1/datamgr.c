/**
*  \author Maxime Schuybroeck
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "datamgr.h"
#define MAX_ROOM_ENTRIES 100
// Define your data structures
typedef struct {
    sensor_id_t sensor_id;
    uint16_t room_id;
    double running_avg;
    time_t last_modified;
} sensor_node_t;

typedef struct {
    uint16_t room_id;
    uint16_t sensor_id;
} room_sensor_mapping_t;

static room_sensor_mapping_t room_sensor_map[MAX_ROOM_ENTRIES];
static int map_size = 0;

void datamgr_parse_sensor_files(FILE *fp_sensor_map, FILE *fp_sensor_data) {
    if(fp_sensor_map == NULL || fp_sensor_data == NULL){
        fprintf(stderr, "Error opening file because they are empty");
    }

    // 'room_sensor.map' file parsen
    uint16_t room_id, sensor_id;

    while (fscanf(fp_sensor_map, "%SCNu16 %SCNu16" , &room_id, &sensor_id) == 2) {
        // Check if the array is not full (you may want to implement dynamic memory allocation)
        if(map_size >= MAX_ROOM_ENTRIES){
            fprintf(stderr, "Error: Too many entries in room_sensor.map");
        }

        // effectief storen in map
        room_sensor_map[map_size].room_id = room_id;
        room_sensor_map[map_size].sensor_id = sensor_id;
        map_size++;
    }

    // Implementation of parsing sensor files
    // ...

    // Example usage of dplist (replace with your linked list implementation)
    dplist_t *sensor_list = dpl_create(sensor_node_copy, sensor_node_free, sensor_node_compare);
    // ...

    // Example iteration through the sensor list (replace with your linked list iteration)
    for (int i = 0; i < dpl_size(sensor_list); ++i) {
        // Access sensor_node_t using dpl_get_element_at_index
        sensor_node_t *node = (sensor_node_t *)dpl_get_element_at_index(sensor_list, i);
        // Process the sensor node as needed
    }

    // Cleanup
    dpl_free(&sensor_list, true);  // Free the linked list and associated data
}

void datamgr_free() {
    // Implementation of cleanup and freeing resources
    // ...
}

uint16_t datamgr_get_room_id(sensor_id_t sensor_id) {
    // Implementation to get the room ID for a given sensor ID
    // ...
}

sensor_value_t datamgr_get_avg(sensor_id_t sensor_id) {
    // Implementation to get the running average for a given sensor ID
    // ...
}

time_t datamgr_get_last_modified(sensor_id_t sensor_id) {
    // Implementation to get the last modified timestamp for a given sensor ID
    // ...
}

int datamgr_get_total_sensors() {
    // Implementation to get the total number of unique sensor IDs recorded
    // ...
}
/**
 * \author Maxime Schuybroeck
 */

#include "sbuffer.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "datamgr.h"
#include "connmgr.h"
#include "lib/dplist.h"
#include "lib/dplist.c"
#include <stdbool.h>
#include "sensor_db.h"


// local variables
sbuffer_t *buffer;
FILE* csv_file;



void* data_manager_thread() {
    // reading data from buffer via the sbuffer_read() function
    bool node_was_found;
    dplist_t list = get_dplist();

    while(1){
        sensor_data_t data;     // to save the data in
        if(sbuffer_read(buffer, &data) != SBUFFER_SUCCESS){
            fprintf(stderr, "Error, in reading the buffer\n")
            break; // while loop stops if end or error is hit
        }

        // updating the dplist
        dplist_node_t *current_node = list.head;
        node_was_found = false;

        while(current_node != NULL){
            if(current_node->element->sensorId == data.id){
                current_node->element->ts = data.ts;
                add_sensor_value(current_node->element->previousValues, data.value);
                current_node->element->average = calculate_avg(current_node->element->previousValues);
                node_was_found = true;
                break;
            }
            current_node = current_node->next;
        }

        // checking if the node was found
        if(!node_was_found){
            fprintf(stderr, "Error, sensor was not found in de dplist\n")
        }
    }
    return NULL;
}

void* storage_manager_thread() {
    // reading data from buffer via the sbuffer_remove()S
    while(1){
        sensor_data_t data;     // to save the data in
        if(sbuffer_remove(buffer, &data) != SBUFFER_SUCCESS){
            break; // while loop stops if end or error is hit
        }

        // writing to the CSV file
        insert_sensor(csv_file, data.id, data.value, data.ts);
    }

    return NULL;
}


int main(int argc, char *argv[]) {
    // first checking if the provided arguments are right
    if(argc != 2) {
        printf("Please provide the right arguments: first the port, then the max nb of clients");
        return -1;
    }

    // initialising the buffer
    if(sbuffer_init(&buffer) != SBUFFER_SUCCESS) {
        fprintf(stderr, "Error initialising the buffer\n");
        return -1;
    }

    // starting the connection manager
    struct connmgr_parameters parameters;
    parameters.server_arguments = argv;
    parameters.buffer = buffer;

    // setting up the data manager
    FILE fp_sensor_map = fopen(room_sensor.map, 'r');
    datamgr_parse_sensor_files(fp_sensor_map);
    dplist_t *list = get_dplist();
    dplist_node_t *current_node = list->head;

    // setting up the storage manager
    csv_file = open_db("data.csv", false);

    // creating the threads
    pthread_t tid[3];
    pthread_create(&tid[0], NULL,start_connmgr, (void *) &parameters);
    pthread_create(&tid[1], NULL, data_manager_thread, NULL);
    pthread_create(&tid[2], NULL, storage_manager_thread, NULL);

    // joining the treads
    for (int i = 0; i < 3; i++) {
        pthread_join(tid[i], NULL);
    }

    // closing CSV file
    close_db(csv_file);
    // freeing the buffer
    sbuffer_free(&buffer);

}
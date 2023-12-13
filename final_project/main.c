/**
 * \author Maxime Schuybroeck
 */

#include "sbuffer.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "datamgr.h"
#include "lib/dplist.h"
#include "lib/dplist.c"
#include <stdbool.h>


// local variables
sbuffer_t *buffer;
FILE* sensor_data;
FILE* csv_file;

sbuffer_t* get_buffer(){
    return *buffer;
}

void* connection_manager_thread() {
    // reading sensor data from the file
    sensor_data_t data;     // to save the data in
    while(fread(&data, sizeof(sensor_data_t), 1, sensor_data) == 1){   // == 1 if reading is still possible

        // inserting data in buffer
        if(sbuffer_insert(buffer, &data) != SBUFFER_SUCCESS){
            fprintf(stderr,"Error inserting the data into the buffer\n");
            break;
        }
    }

    // adding the end-of-stream marker to the buffer
    data.id = 0;    // making an indication
    if(sbuffer_insert(buffer, &data) != SBUFFER_SUCCESS){
        fprintf(stderr, "Error in inserting the end-of-stream marker into the buffer\n");
    }

    return NULL;
}

void* data_manager_thread() {
    // setting up the data manager
    FILE fp_sensor_map = fopen(room_sensor.map, 'r');
    datamgr_parse_sensor_files(fp_sensor_map);
    dplist_t *list = get_dplist();
    dplist_node_t *current_node = list->head;

    // reading data from buffer via the sbuffer_read() function
    bool node_was_found;
    while(1){
        sensor_data_t data;     // to save the data in
        if(sbuffer_read(buffer, &data) != SBUFFER_SUCCESS){
            break; // while loop stops if end or error is hit
        }

        // updating the dplist
        node_was_found = false;
        while(current_node != NULL){
            if(current_node->element->sensorId == data.id){
                current_node->element->ts = data.ts;
                current_node->element->average = (current_node->element->average + data.value)/2;
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
    // reading data from buffer via the sbuffer_remove()
    while(1){
        sensor_data_t data;     // to save the data in
        if(sbuffer_remove(buffer, &data) != SBUFFER_SUCCESS){
            break; // while loop stops if end or error is hit
        }

        // writing to the CSV file
        fprintf(csv_file, "%u, %lf, %li\n", data.id, data.value, data.ts);
    }

    return NULL;
}



int start_threads(){
    // opening the sensor_data file to read
    sensor_data = fopen("sensor_data", "rb");
    if(sensor_data == NULL) {
        fprintf(stderr,"Error opening file because it is empty\n");
        return -1;
    }

    // opening CSV file to write in
    csv_file = fopen("sensor_data_out.csv", "w");
    if (csv_file == NULL) {
        fprintf(stderr, "Error in opening the CSV file because it is NULL\n");
        return -1;
    }

    pthread_t connmgr, datamgr, storemgr;

    // creating the threads
    if(pthread_create(&connmgr, NULL, connection_manager_thread, (void *)buffer) != 0 ||
       pthread_create(&datamgr, NULL, data_manager_thread, (void *)buffer) != 0 ||
       pthread_create(&storemgr, NULL, storage_manager_thread, (void *)buffer) != 0) {
        return -1;  // if != 0 --> error --> -1
    }

    // joining the treads
    pthread_join(connmgr, NULL);
    pthread_join(datamgr, NULL);
    pthread_join(storemgr, NULL);

    // closing the file
    fclose(sensor_data);
    // closing CSV file
    fclose(csv_file);
    // freeing the buffer
    sbuffer_free(&buffer);

    return 0;       // Success
}

int main() {
    // initialising the buffer
    if(sbuffer_init(&buffer) != SBUFFER_SUCCESS) {
        fprintf(stderr, "Error initialising the buffer\n");
        return -1;
    }

    // starting the threads
    if(start_threads() != 0) {
        fprintf(stderr, "Error initialising the threads\n");
        return -1;
    } else{
        sbuffer_free(&buffer);
        return 0;
    }

}
/**
 * \author Maxime Schuybroeck
 */

#include "threads.h"
#include "sbuffer.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // usleep
#include <string.h>
#include <errno.h>

// local variables
sbuffer_t *buffer;
FILE* sensor_data;
FILE* csv_file;
pthread_mutex_t myMutex;


void* writer_thread(void *arg) {
    buffer = (sbuffer_t *)arg;  // Cast the argument back to sbuffer_t*

    // reading sensor data from the file
    sensor_data_t data;     // to save the data in
    while(fread(&data, sizeof(sensor_data_t), 1, sensor_data) == 1){   // == 1 if reading is still possible
        // locking the mutex before accessing
        /*
        if (pthread_mutex_lock(&buffer->mutex) != 0) {
            fprintf(stderr, "Error locking mutex in the writer_thread\n");
            break;
        }
         */
        // inserting data in buffer
        if(sbuffer_insert(buffer, &data) != SBUFFER_SUCCESS){
            fprintf(stderr,"Error inserting the data into the buffer\n");

            // unlocking the mutex before breaking
            //pthread_mutex_unlock(&buffer->mutex);
            break;
        }

        // unlock the mutex after inserting the data
        //pthread_mutex_unlock(&buffer->mutex);

        // waiting or sleeping for 10 milliseconds
        usleep(10000);
    }

    // adding the end-of-stream marker to the buffer
    data.id = 0;    // making an indication
    if(sbuffer_insert(buffer, &data) != SBUFFER_SUCCESS){
        fprintf(stderr, "Error in inserting the end-of-stream marker into the buffer\n");
    }

    return NULL;
}

void* reader_thread(void *arg) {
    //
    buffer = (sbuffer_t *)arg;  // Cast the argument back to sbuffer_t*

    // reading data from buffer via the sbuffer_remove()
    while(1){
        sensor_data_t data;     // to save the data in
        if(sbuffer_remove(buffer, &data) != SBUFFER_SUCCESS){
            fprintf(stderr, "Error in removing data from the buffer or end-of-stream marker was encountered\n");
            break; // while loop stops if end or error is hit
        }

        // writing to the CSV file
        pthread_mutex_lock(&(&buffer).lock);
        fprintf(csv_file, "%u, %.2f, %ld\n", data.id, data.value, (long)data.ts);
        pthread_mutex_unlock((&&buffer)->lock);

        // Waiting or sleeping for 25 milliseconds
        usleep(25000);
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

    pthread_t writer, reader1, reader2;

    // creating the threads
    if(pthread_create(&writer, NULL, writer_thread, (void *)buffer) != 0 ||
       pthread_create(&reader1, NULL, reader_thread, (void *)buffer) != 0 ||
       pthread_create(&reader2, NULL, reader_thread, (void *)buffer) != 0) {
        return -1;  // if != 0 --> error --> -1
    }

    // joining the treads
    pthread_join(writer, NULL);
    pthread_join(reader1, NULL);
    pthread_join(reader2, NULL);

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

    // initialising the mutex
    pthread_mutex_t myMutex;


    // starting the threads
    if(start_threads() != 0) {
        fprintf(stderr, "Error initialising the threads\n");
        return -1;
    } else{
        sbuffer_free(&buffer);
        return 0;
    }

}

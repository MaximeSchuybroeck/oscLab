/**
 * \author Maxime Schuybroeck
 */

#include <stdio.h>
#include <stdbool.h>
#include "config.h"
#include "sensor_db.h"


// variables
FILE *db;


FILE * open_db(char * filename, bool append){
    db = fopen(filename, append ? "a" : "w");

    // checking if the file is empty
    if(db == NULL){
        printf("Error in opening the db file because it is EMPTY\n");
        return db;
    }

    // logging
    char *msg = "A new data.csv file has been created.\n";
    write_to_log_process(msg);

    return db;
}

int insert_sensor(FILE * f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts){
    // checking if the file is empty
    if(f == NULL){
        printf("Error in opening the file because it is EMPTY\n");
        return -1;
    }
    // log message
    char msg[55];
    snprintf(msg, sizeof(msg), "Data insertion from sensor %d succeeded\n", id);

    // writing to the csv file
    if(fprintf(f, "%u, %.6lf, %ld\n", id, value, ts) == -1){
        snprintf(msg, sizeof(msg), "An error occurred when writing to the csv file.\n");
    }

    // logging
    write_to_log_process(msg);
    return 0;
}

int close_db(FILE * f){
    // checking if the file is empty
    if(f == NULL){
        printf("Error in opening the file because it is EMPTY\n");
        return -1;
    }
    // closing the file
    fclose(f);

    // logging
    char msg[55];
    snprintf(msg, sizeof(msg), "The data.csv file has been closed\n");
    write_to_log_process(msg);

    return 0;
}
/**
 * \author Maxime Schuybroeck
 */

#include "config.h"


// variables
FILE *db;


FILE * open_db(char * filename, bool append){
    db = fopen(filename, append ? "a" : "w");

    // checking if the file is empty
    if(db == NULL){
        printf("Error in opening the db file because it is EMPTY\n");
        return db;
    }
    // Create the log process
    create_log_process();

    // logging
    char *msg = "Data log_file opened.\n";
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
    char *msg = "Data inserted.\n";

    // writing to the csv file
    if(fprintf(f, "%u, %.6lf, %ld\n", id, value, ts) == -1){
        msg = "An error occurred when writing to the csv file.\n";
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
    char *msg = "CSV file has been closed.\n";
    write_to_log_process(msg);

    // closing the log_file
    end_log_process();
    return 0;
}
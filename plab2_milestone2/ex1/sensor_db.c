/**
 * \author Maxime Schuybroeck
 */

#include "sensor_db.h"

FILE * open_db(char * filename, bool append){
    FILE *file = NULL;
    if(append){
        //append mode = the file openen om aan het einde aan te passen
        file = fopen(filename, "a");
    } else{
        // overwrite mode = file openen om te writen
        file = fopen(filename, "w");
    }
    if(file == NULL){
        perror("File is empty, nothing to empty");
    }
    return file;
}

int insert_sensor(FILE * f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts){
    if(f == NULL){
        perror("File is empty");
        return -1;
    }
    //writing to the file
    fprintf(f,"%u, %.6lf, %ld\n", id, value, ts);
    if(ferror(f) != 0){
        perror("Writing to file error");
        return -1;
    }
    return 0; // = succes
}


int close_db(FILE * f){
    if(f == NULL){
        perror("File is empty, nothing to close");
        return -1;
    }

    // Close the file
    if (fclose(f) != 0) {
        perror("Error in closing file");
        return -1; // Return an error code
    }

    return 0; // Return 0 on success

}
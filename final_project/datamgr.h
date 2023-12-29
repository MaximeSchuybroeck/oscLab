/**
 * \author {AUTHOR}
 */

#ifndef DATAMGR_H_
#define DATAMGR_H_

#ifndef SET_MAX_TEMP
#error SET_MAX_TEMP not set
#endif

#ifndef SET_MIN_TEMP
#error SET_MIN_TEMP not set
#endif

#include <stdlib.h>
#include <stdio.h>
#include "config.h"


/**
 *  This method holds the core functionality of your datamgr. It takes in 2 file pointers to the sensor files and parses them. 
 *  When the method finishes all data should be in the internal pointer list and all log messages should be printed to stderr.
 *  \param fp_sensor_map file pointer to the map file
 *  \param fp_sensor_data file pointer to the binary data file
 */
int datamgr_parse_room_sensor_map();

/** This method calculates the average of the values inside of the array
 * \param valueList the array with the previous values
 * \return the average
 */
sensor_value_t calculate_avg(sensor_value_t valueList[RUN_AVG_LENGTH]);

/** This method returns the number of elements inside of the array, that are not 0
 * \param valueList with the previous sensor values
 * @return the number of elements inside of the array, that are not 0
 */
int get_valuelist_size(sensor_value_t valueList[RUN_AVG_LENGTH]);

/**
 * This method should be called to clean up the datamgr, and to free all used memory. 
 * After this, any call to datamgr_get_room_id, datamgr_get_avg, datamgr_get_last_modified or datamgr_get_total_sensors will not return a valid result
 */
void datamgr_free();

/**
 * The datamgr thread
*/
void *data_manager_thread();

#endif  //DATAMGR_H_
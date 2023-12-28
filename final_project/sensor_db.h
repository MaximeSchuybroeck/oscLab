/**
 * \author Bert Lagaisse
 */

#ifndef _SENSOR_DB_H_
#define _SENSOR_DB_H_

#include <stdio.h>
#include <stdlib.h>
#include "config.h"

#include <stdbool.h>
int open_db();
int insert_sensor(sensor_id_t id, sensor_value_t value, sensor_ts_t ts);
int close_db();
void* storage_manager_thread();


#endif /* _SENSOR_DB_H_ */
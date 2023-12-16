/**
 * \author Maxime Schuybreock
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifndef RUN_AVG_LENGTH
#define RUN_AVG_LENGTH 5
#endif


#ifndef SET_MAX_TEMP
#define SET_MAX_TEMP 20
#endif

#ifndef SET_MIN_TEMP
#define SET_MIN_TEMP 10
#endif

#include <stdint.h>
#include <time.h>
#include <stdbool.h>


typedef uint16_t sensor_id_t;
typedef uint16_t room_id_t;
typedef double sensor_value_t;
typedef time_t sensor_ts_t;


typedef struct {
    sensor_id_t id;
    sensor_value_t value;
    sensor_ts_t ts;
    bool read_by_datamgr;
} sensor_data_t;

struct element {
    sensor_id_t sensorId;
    room_id_t roomId;
    sensor_value_t previousValues[RUN_AVG_LENGTH];
    sensor_ts_t ts;
};


typedef struct element element_t;


#endif /* _CONFIG_H_ */

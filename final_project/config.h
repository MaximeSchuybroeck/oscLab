/**
 * \author Maxime Schuybreock
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifndef RUN_AVG_LENGTH
#define RUN_AVG_LENGTH 5
#endif

#define SET_MIN_TEMP
#define SET_MAX_TEMP

//TODO: code wegdoen
/*
#ifndef SET_MAX_TEMP
#error SET_MAX_TEMP 20
#endif

#ifndef SET_MIN_TEMP
#error SET_MIN_TEMP 10
#endif
 */

#include <stdint.h>
#include <time.h>
#include <stdbool.h>

struct sbuffer;
typedef struct sbuffer sbuffer_t;


typedef uint16_t sensor_id_t;
typedef uint16_t room_id_t;
typedef double sensor_value_t;
typedef time_t sensor_ts_t;         // UTC timestamp as returned by time() - notice that the size of time_t is different on 32/64 bit machine


typedef struct {
    sensor_id_t id;
    sensor_value_t value;
    sensor_ts_t ts;
    bool read_by_datamgr;
} sensor_data_t;

struct connmgr_parameters{
    char *server_arguments[3];
    sbuffer_t buffer;
};

#endif /* _CONFIG_H_ */

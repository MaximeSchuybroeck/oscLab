/**
 * \author Maxime Schuybreock
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifndef RUN_AVG_LENGTH
#define RUN_AVG_LENGTH 5

#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "lib/tcpsock.h"
#include "lib/dplist.h"
#include <pthread.h>
#include "datamgr.h"
#include <stdbool.h>
#include <assert.h>
#include "sbuffer.h"
#include "connmgr.h"
#include "sensor_db.h"
#include "logger.h"
#include <unistd.h>




typedef uint16_t sensor_id_t;
typedef uint16_t room_id_t;
typedef double sensor_value_t;
typedef time_t sensor_ts_t;         // UTC timestamp as returned by time() - notice that the size of time_t is different on 32/64 bit machine


typedef struct {
    sensor_id_t id;
    sensor_value_t value;
    sensor_ts_t ts;
} sensor_data_t;

#endif /* _CONFIG_H_ */

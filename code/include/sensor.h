#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>

// Temprature state
//   Tracking the state of the sensors.
//   Past temps.

// Temprature functions
//   Get temps and stuff 

// Sanity checks:
//  To low temp
//  To high temp
//  No change after heating action
//  Abnormal deviations
//  A to big delta between sensor 1 & 2


#define P_SENSOR_MAX_OUTLIER_COUNT 5
#define P_SENSOR_OUTLIER_COUNT_RST 10

// 0 -- 1 -- 2 -- 3 -- 4 -- 5 -- 6 -- 7 --  8 -- 9 

uint8_t p_temp_sensor_initialized = 0;

typedef struct {
    uint16_t* sensor_one_readings;
    //uint16_t* temp_sensor_one_deviations;

    uint16_t* sensor_two_readings;
    //uint16_t* temp_sensor_two_deviations;

    uint8_t sensor_one_outlier_cnt;
    uint8_t sensor_two_outlier_cnt;

    uint16_t cur_reading_index;
    uint16_t max_reading_history;
} temp_sensor_state;

int p_sensor_init(temp_sensor_state* sensor_state_ptr, uint16_t max_history);



#endif
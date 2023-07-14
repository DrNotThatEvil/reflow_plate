#ifndef T_SENSOR_H
#define T_SENSOR_H

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

#define T_VCC 5
#define T_SENSOR_R 100000 // 100K
#define T_SERIES_R 100000 // 100K

#define T_SENSOR_MAX_OUTLIER_COUNT 5
#define T_SENSOR_OUTLIER_COUNT_RST 10

// 0 -- 1 -- 2 -- 3 -- 4 -- 5 -- 6 -- 7 --  8 -- 9 

uint8_t t_sensor_initialized = 0;

typedef struct {
    uint16_t* sensor_one_readings;
    float sensor_one_abc[3];
    //uint16_t* temp_sensor_one_deviations;

    uint16_t* sensor_two_readings;
    float sensor_two_abc[3];
    //uint16_t* temp_sensor_two_deviations;

    uint8_t sensor_one_outlier_cnt;
    uint8_t sensor_two_outlier_cnt;

    uint8_t cur_reading_index;
    uint8_t max_reading_history;
} t_sensor_state;

void t_sensor_init(
    t_sensor_state* state_ptr,
    float a1, float b1, float c1,
    float a2, float b2, float c2,
    uint8_t max_history
);

void t_sensor_tick(t_sensor_state* state_ptr);

#endif
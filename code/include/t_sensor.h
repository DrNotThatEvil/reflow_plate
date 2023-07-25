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

#define T_SENSOR0_NOMR 100000       // 100K resistance at 25 c
#define T_SENSOR0_NOMT 298.15f       // temp for norminal resistance in kelvin
#define T_SENSOR0_SERIESR 100000    // 100K series resistor
#define T_SENSOR0_BCOEFF 3950       // temp for norminal resistance

#define T_SENSOR1_NOMR 100000       // 100K resistance at 25 c
#define T_SENSOR1_NOMT 298.15f      // temp for norminal resistance in kelvin
#define T_SENSOR1_SERIESR 100000    // 100K series resistor
#define T_SENSOR1_BCOEFF 3950       // temp for norminal resistance

#define T_SENSOR_MAX_OUTLIER_COUNT 5
#define T_SENSOR_OUTLIER_RST_COUNT 5

#define T_SENSOR_BAD_MIN 10
#define T_SENSOR_BAD_MAX 200
#define T_SENSOR_BAD_MAX_DIFF 5

// 0 -- 1 -- 2 -- 3 -- 4 -- 5 -- 6 -- 7 --  8 -- 9 

uint8_t t_sensor_initialized = 0;
struct t_sensor_state;

typedef void (*t_sensor_error_cb)(struct t_sensor_state* state_ptr);

typedef enum t_reading_state {
    PREPARING,
    NORMAL,
    HAS_OUTLIERS,
    ERRORED
} t_reading_state;

typedef struct {
    uint16_t* s_readings_0;
    uint16_t* s_readings_1;
    uint16_t** cur_reading_list;

    uint8_t s_outlier_cnt;
    uint8_t s_outlier_rst_cnt;

    uint8_t cur_reading_index;
    uint8_t max_reading_history;

    t_reading_state reading_state;
    t_sensor_error_cb error_cb;
} t_sensor_state;

void t_sensor_init(
    t_sensor_state* state_ptr,
    uint8_t max_history,
    t_sensor_error_cb error_cb
);

int t_sensor_sanitycheck(t_sensor_state* state_ptr, float s0_temp, float s1_temp);
float t_sensor_read_raw(t_sensor_state* state_ptr, int sensor);

void t_sensor_tick(t_sensor_state* state_ptr);


#endif
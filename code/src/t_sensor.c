#include "t_sensor.h"

void t_sensor_init(
    t_sensor_state* state_ptr, 
    float a1, float b1, float c1,
    float a2, float b2, float c2,
    uint8_t max_history
)
{
    state_ptr->cur_reading_index = 0;
    state_ptr->sensor_one_outlier_cnt = 0;
    state_ptr->sensor_two_outlier_cnt = 0;

    state_ptr->max_reading_history = max_history;

    state_ptr->sensor_one_readings = malloc(sizeof(uint16_t) * max_history);
    state_ptr->sensor_two_readings = malloc(sizeof(uint16_t) * max_history);
}

void t_sensor_tick(t_sensor_state* state_ptr)
{
    
}
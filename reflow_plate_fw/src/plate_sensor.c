#include "plate_sensor.h"
#include <stdlib.h>

int p_sensor_init(temp_sensor_state* sensor_state_ptr, uint16_t max_history)
{
    sensor_state_ptr->max_reading_history = max_history;
    sensor_state_ptr->cur_reading_index = 0;
    sensor_state_ptr->sensor_one_outlier_cnt = 0;
    sensor_state_ptr->sensor_two_outlier_cnt = 0;

    sensor_state_ptr->sensor_one_readings = (uint16_t*) malloc(sizeof(uint16_t) * max_history);
    sensor_state_ptr->sensor_two_readings = (uint16_t*) malloc(sizeof(uint16_t) * max_history);

    return 1;
}

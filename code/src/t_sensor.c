#include "t_sensor.h"

#include <pico/stdlib.h>
#include <hardware/gpio.h>
#include <hardware/adc.h>
#include <stdlib.h>
#include <math.h>

void t_sensor_init(
    t_sensor_state* state_ptr,
    uint8_t max_history,
    t_sensor_error_cb error_cb
)
{
    state_ptr->cur_reading_index = 0;
    state_ptr->s_outlier_cnt = 0;
    state_ptr->s_outlier_rst_cnt = 0;

    state_ptr->max_reading_history = max_history;
    state_ptr->s0_readings = malloc(sizeof(uint16_t) * max_history);
    state_ptr->s1_readings = malloc(sizeof(uint16_t) * max_history);

    state_ptr->sanity_state = NORMAL;
    state_ptr->error_cb = error_cb;

    for(uint8_t i = 0; i < max_history; i++)
    {
        state_ptr->s0_readings[i] = 0;
        state_ptr->s1_readings[i] = 0;
    }
}

int t_sensor_sanitycheck(t_sensor_state *state_ptr, float s0_temp, float s1_temp)
{
    int sensor_diff = abs(((int)s0_temp) - ((int)s1_temp));

    if (s0_temp < T_SENSOR_BAD_MIN || s0_temp > T_SENSOR_BAD_MAX || s1_temp < T_SENSOR_BAD_MIN || s1_temp > T_SENSOR_BAD_MAX)
    {
        state_ptr->s_outlier_cnt++;
        state_ptr->sanity_state = HAS_OUTLIERS;
        state_ptr->s_outlier_rst_cnt = 0; // bad readings reset the good recovery counter

        if(state_ptr->s_outlier_cnt > T_SENSOR_MAX_OUTLIER_COUNT) {
            // Call safety routine.
            state_ptr->sanity_state = ERRORED;
            state_ptr->error_cb(state_ptr);
        }

        return 1;
    }

    if(sensor_diff > T_SENSOR_BAD_MAX_DIFF)
    {
        state_ptr->s_outlier_cnt++;
        state_ptr->sanity_state = HAS_OUTLIERS;
        state_ptr->s_outlier_rst_cnt = 0; // bad readings reset the good recovery counter

        if(state_ptr->s_outlier_cnt > T_SENSOR_MAX_OUTLIER_COUNT) {
            // Call safety routine.
            state_ptr->sanity_state = ERRORED;
            state_ptr->error_cb(state_ptr);
        }

        return 1;
    }

    if (state_ptr->s_outlier_cnt > 0)
    {
        // Good reading but still recovering from bad ones.
        state_ptr->s_outlier_rst_cnt++;

        if (state_ptr->s_outlier_rst_cnt > T_SENSOR_OUTLIER_RST_COUNT) {
            // reset counters.
            state_ptr->sanity_state = NORMAL;
            state_ptr->s_outlier_cnt = 0;
            state_ptr->s_outlier_rst_cnt = 0;
        }

        return 1;
    }

    return 0;
}

float t_sensor_read_raw(t_sensor_state* state_ptr, int sensor)
{
    adc_select_input(sensor);

    float s_read = adc_read();
    s_read = T_SENSOR0_SERIESR / ((4095 / s_read) - 1);

    return (1.0f / ((log(s_read / T_SENSOR0_NOMR) / T_SENSOR0_BCOEFF) + 1.0f / T_SENSOR0_NOMT)) - 273.15f;
}

void t_sensor_tick(t_sensor_state *state_ptr)
{
    float s0 = t_sensor_read_raw(state_ptr, 0);
    float s1 = t_sensor_read_raw(state_ptr, 1);

    int sanity_check = t_sensor_sanitycheck(state_ptr, s0, s1);
    if (sanity_check > 0)
    {
        // Sanity check failed
        // Check if errored that kind of stuff.
        return;
    }

}
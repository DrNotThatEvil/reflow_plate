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

    state_ptr->s_roc_0 = 0.f;
    state_ptr->s_roc_1 = 0.f;
    state_ptr->s_cur_roc = &state_ptr->s_roc_0;
    state_ptr->s_pre_roc = &state_ptr->s_roc_1;

    state_ptr->max_reading_history = max_history;
    state_ptr->s_readings_0 = malloc(sizeof(uint16_t) * max_history);
    state_ptr->s_readings_1 = malloc(sizeof(uint16_t) * max_history);
    state_ptr->cur_reading_list = &state_ptr->s_readings_0;
    state_ptr->pre_reading_list = &state_ptr->s_readings_1;

    state_ptr->reading_state = PREPARING;
    state_ptr->error_cb = error_cb;

    for(uint8_t i = 0; i < max_history; i++)
    {
        state_ptr->s_readings_0[i] = 0;
        state_ptr->s_readings_1[i] = 0;
    }
}

int t_sensor_sanitycheck(t_sensor_state *state_ptr, float s0_temp, float s1_temp)
{
    int sensor_diff = abs(((int)s0_temp) - ((int)s1_temp));

    if (s0_temp < T_SENSOR_BAD_MIN || s0_temp > T_SENSOR_BAD_MAX || s1_temp < T_SENSOR_BAD_MIN || s1_temp > T_SENSOR_BAD_MAX)
    {
        state_ptr->s_outlier_cnt++;
        state_ptr->reading_state = HAS_OUTLIERS;
        state_ptr->s_outlier_rst_cnt = 0; // bad readings reset the good recovery counter

        if(state_ptr->s_outlier_cnt > T_SENSOR_MAX_OUTLIER_COUNT) {
            // Call safety routine.
            state_ptr->reading_state = ERRORED;
            state_ptr->error_cb(state_ptr);
        }

        return 1;
    }

    if(sensor_diff > T_SENSOR_BAD_MAX_DIFF)
    {
        state_ptr->s_outlier_cnt++;
        state_ptr->reading_state = HAS_OUTLIERS;
        state_ptr->s_outlier_rst_cnt = 0; // bad readings reset the good recovery counter

        if(state_ptr->s_outlier_cnt > T_SENSOR_MAX_OUTLIER_COUNT) {
            // Call safety routine.
            state_ptr->reading_state = ERRORED;
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
            state_ptr->reading_state = PREPARING;
            state_ptr->s_outlier_cnt = 0;
            state_ptr->s_outlier_rst_cnt = 0;

            for(uint8_t i = 0; i < state_ptr->max_reading_history; i++)
            {
                state_ptr->s_readings_0[i] = 0;
                state_ptr->s_readings_1[i] = 0;
            }

            state_ptr->cur_reading_list = &state_ptr->s_readings_0;
        }

        return 1;
    }

    return 0;
}

float t_sensor_read_raw(t_sensor_state* state_ptr, int sensor)
{
    adc_select_input(sensor);

    float s_read = adc_read();
    //s_read = T_SENSOR0_SERIESR / ((4095 / s_read) - 1);
    s_read = (4095.0f - s_read) * T_SENSOR0_SERIESR / s_read;
    //s_read = T_SENSOR0_SERIESR * s_read;

    float temp = 1.0f / (log(s_read/T_SENSOR0_SERIESR) / T_SENSOR0_BCOEFF + 1.0f / 298.15f) - 273.15f;

/*
    float steinhart;
    steinhart = s_read / T_SENSOR0_NOMR;
    steinhart = log(steinhart);
    steinhart /= T_SENSOR0_BCOEFF;
    steinhart += 1.0f / T_SENSOR0_NOMT;
    steinhart = 1.0f / steinhart;
    steinhart -= 273.15f;

    return steinhart;
    */
    return temp;

    //return (1.0f / ((log(s_read / T_SENSOR0_NOMR) / T_SENSOR0_BCOEFF) + 1.0f / T_SENSOR0_NOMT)) - 273.15f;
}

float t_sensor_avg_rate_of_change(t_sensor_state *state_ptr)
{
    float t_rate_of_change = 0.0f;
    for(uint8_t i = 0; i < state_ptr->max_reading_history; i++)
    {
        t_rate_of_change += (((*state_ptr->cur_reading_list)[i] - (*state_ptr->cur_reading_list)[i - 1]) / 100.0f);
    }

    return t_rate_of_change / (state_ptr->max_reading_history - 1);
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

    // Add average of the 2 sensors to the current list.
    (*state_ptr->cur_reading_list)[state_ptr->cur_reading_index] = (s0 + s1) / 2;
    state_ptr->cur_reading_index = (state_ptr->cur_reading_index + 1) % state_ptr->max_reading_history;
    if (state_ptr->cur_reading_index == 0)
    {
        // (cur_reading_index + 1) % max
        // once it flips the current list can be flipped.
        state_ptr->reading_state = NORMAL;
        float rateOfChange = t_sensor_avg_rate_of_change(state_ptr);

        uint16_t** tmp = state_ptr->pre_reading_list;
        state_ptr->pre_reading_list = state_ptr->cur_reading_list;
        state_ptr->cur_reading_list = tmp;

        float* tmp_f = state_ptr->s_pre_roc;
        (*state_ptr->s_cur_roc) = rateOfChange;
        state_ptr->s_pre_roc = state_ptr->s_cur_roc;
        state_ptr->s_cur_roc = tmp_f;
    }

    // Check if enough readings have been made.
    if (state_ptr->reading_state != NORMAL)
    {
        return;
    }

}
#include <stdio.h>
#include <stdlib.h>
#include <pico/stdlib.h>
#include <hardware/gpio.h>
#include <hardware/adc.h>

#include "t_sensor.h"

void t_error_cb(t_sensor_state* state_ptr)
{

}

int main()
{
    stdio_init_all();
    adc_init();

    adc_gpio_init(26);
    adc_gpio_init(27);

    t_sensor_state* t_state_ptr = malloc(sizeof(t_sensor_state));
    t_sensor_init(t_state_ptr, 5, &t_error_cb);

    while (1) {
        t_sensor_tick(t_state_ptr);
        
        if(t_state_ptr->reading_state == NORMAL || t_state_ptr->reading_state == HAS_OUTLIERS)
        {
            printf("------------------------------------------------------\n");
            for(uint8_t i = 0; i < t_state_ptr->max_reading_history; i++)
            {
                printf("Temp: %d\n", (*t_state_ptr->pre_reading_list)[i]);
            }
            printf("Avg rate of change: %.2f\n", *(t_state_ptr->s_cur_roc));
            printf("------------------------------------------------------\n");
        }


        if(t_state_ptr->reading_state == HAS_OUTLIERS)
        {
            printf("Has outliers you dumbo!\n");
        }

        sleep_ms(100);
    }
}
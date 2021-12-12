#include <avr/io.h>
// FreeRTOS
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "display.h"
#include "adc.h"
#include "string.h"
#include "stdio.h"
#include "adc0values.h"

struct lcd_message adc0_value_message;
uint16_t value = 0;
char str[17];


void adc0_value_init(void)
{
    adc0_value_message.xpos = 0;
    adc0_value_message.ypos = 0;
    sprintf(str, "LDR value: %d", value);
    strcpy(adc0_value_message.text, str);
    adc0_value_queue = xQueueCreate(1, sizeof(adc0_value_message));
    xQueueSend(
                adc0_value_queue,
                &adc0_value_message,
                1
               );
}

void adc0_value_task()
{
    adc0_value_init();
    vTaskDelay(pdMS_TO_TICKS(200));
    uint8_t i = 0; 
    while (1)
    {
        if (i == 0)
        {
            value = ldr_read();
            sprintf(str, "LDR value: %d\r\n", value);
            i++;
        }
        else if (i == 1)
        {
            value = thermistor_read();
            sprintf(str, "NTC value: %d\r\n", value);
            i++;
        }
        else
        {
            value = potentiometer_read();
            sprintf(str, "POT value: %d\r\n", value);
            i = 0;
        }
        strcpy(adc0_value_message.text, str);
        xQueueSend(
                    adc0_value_queue,
                    &adc0_value_message,
                    1
                  );
        
        vTaskDelay(pdMS_TO_TICKS(660));
    }
}

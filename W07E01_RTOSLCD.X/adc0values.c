/* 
 * File:   adc0values.c
 * Author: Juuso Pyykkönen
 * 
 * First reads LDR value and sends it as a message to LCD.
 * Then thermistor value.
 * Then potentiometer.
 * Then starts over.
 * Messages are sent in ~660ms intervals.
 *
 * Created on December 12, 2021, 2:35 PM
 */


#include <avr/io.h>
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "adc0values.h"
#include "display.h"
#include "adc.h"
#include "string.h"
#include "stdio.h"

// Struct from display.h.
// Used for sending messages to LCD.
struct lcd_message adc0_value_message;

// LDR value
uint16_t value = 0;

// Message text
char str[17];


void adc0_value_init(void)
{
    // Message position on LCD
    adc0_value_message.xpos = 0;
    adc0_value_message.ypos = 0;
    
    // Message queue for ADC values
    adc0_value_queue = xQueueCreate(1, sizeof(adc0_value_message));
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
            sprintf(str, "LDR value: %d   ", value);
            i++;
        }
        else if (i == 1)
        {
            value = thermistor_read();
            sprintf(str, "NTC value: %d   ", value);
            i++;
        }
        else
        {
            value = potentiometer_read();
            sprintf(str, "POT value: %d   ", value);
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

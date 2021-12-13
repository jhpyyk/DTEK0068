/* 
 * File:   dummy.c
 * Author: Juuso Pyykkönen
 * 
 * Task that sets on-board LED on if thermistor value is greater
 * than potentiometer. Otherwise turns the LED off.
 * Checks the condition every 100ms.
 *
 * Created on December 7, 2021, 4:04 PM
 */

#include <avr/io.h>
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "adc.h"


void dummy_task()
{
    // Set PF5 as output (on-board LED) 
    PORTF.DIRSET = PIN5_bm;
    
    vTaskDelay(pdMS_TO_TICKS(200));
    
    while (1)
    {
        if (thermistor_read() > potentiometer_read())
        {
            PORTF.OUTSET = PIN5_bm;
        }
        else
        {
            PORTF.OUTCLR = PIN5_bm;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
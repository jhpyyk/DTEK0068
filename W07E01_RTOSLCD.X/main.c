/*
 * File:   main.c
 * Author: Juuso Pyykkönen
 * Email: juuso.pyyk@gmail.com
 * 
 * Exercise for a university course, DTEK0068
 * 
 * Week 7 exercise 1, W07E01
 * 
 * Tasks in this program do following things:
 * - Send messages to serial terminal in one second intervals
 * - Adjust LCD backlight relative to LDR value
 * - Turn backlight off after 10 seconds without potentiometer change
 * - Turn on-board LED on when NTC thermistor is greater than
 *   potentiometer value and off otherwise
 * - Display LDR, thermistor and potentiometer values in LCD
 *   one after the other
 * - Display a back and forth scrolling text in LCD
 * 
 * 
 * Created on December 7, 2021, 4:02 PM
 */
#include <avr/io.h>
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "clock_config.h"
#include "task.h"

#include "uart.h"
#include "adc.h"
#include "backlight.h"
#include "display.h"
#include "scroller.h"
#include "adc0values.h"
#include "dummy.h"


int main(void)
{
    
    usart0_init();
    adc0_init();
    backlight_init();
    scroll_init();
    
    
    // Create a task to send string to the serial terminal via USART0
    // in one second intervals
    xTaskCreate(
                usart0_send_string_task,
                "usart0_send_string_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY,
                NULL
               );
    
    xTaskCreate(
                backlight_adjust_task,
                "backlight_adjust_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY,
                NULL
               );
    xTaskCreate(
                scroll_task,
                "scroll_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY,
                NULL
               );
    xTaskCreate(
                adc0_value_task,
                "adc0_value_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY,
                NULL
               );
    xTaskCreate(
                lcd_send_message_task,
                "lcd_send_message_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY,
                NULL
               );
    xTaskCreate(
                dummy_task,
                "dummy_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                2,
                NULL
               );
    
    // Start task scheduler
    vTaskStartScheduler();
    
    // Scheduler will not return
    return 0;
}

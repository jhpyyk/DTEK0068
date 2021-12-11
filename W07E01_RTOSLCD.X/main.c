/*
 * File:   main.c
 * Author: juuso
 *
 * Created on December 7, 2021, 4:02 PM
 */
#include <avr/io.h>
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "clock_config.h"
#include "task.h"
#include "timers.h"

#include "uart.h"
#include "adc.h"
#include "backlight.h"
#include "lcd.h"
#include "util/delay.h"
#include "display.h"

TaskHandle_t init_handle;

void init_task()
{
    usart0_init();
    adc0_init();
    backlight_init();
    lcd_init();
    display_init();
    vTaskDelay(pdMS_TO_TICKS(200));
}

int main(void)
{
    xTaskCreate(
                init_task,
                "init_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY,
                NULL
               );
    
    
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
                lcd_send_message_task,
                "lcd_send_message_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY,
                NULL
               );
    
    // Start task scheduler
    vTaskStartScheduler();
    
    // Scheduler will not return
    return 0;
}

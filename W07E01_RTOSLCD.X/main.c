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

#include "uart.h"

int main(void)
{
    usart0_init();
    
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
    
    // Start task scheduler
    vTaskStartScheduler();
    
    // Scheduler will not return
    return 0;
}

#include <avr/io.h>
// FreeRTOS
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

#include "lcd.h"
#include "queue.h"

#include "display.h"
#include "stdio.h"

QueueHandle_t message_queue;

struct lcd_message message;

void display_init(void)
{
    message.xpos = 0;
    message.ypos = 1;
    sprintf(message.text, "ssss");
    char str[6] = "sssd";
    message_queue = xQueueCreate(10, sizeof(str));
    xQueueSend(message_queue,
               (void *)str,
               1);
}

void lcd_send_message_task()
{
    char str[6];
    
    vTaskDelay(pdMS_TO_TICKS(200));
    
    while (1)
    {
        if(message_queue != NULL)
        {
            if (xQueueReceive(message_queue, &(str), 0) == pdPASS)
            {
                lcd_write(str);
                str[0] = '\0';
            }
        }
    }
}

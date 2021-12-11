#include <avr/io.h>
// FreeRTOS
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "lcd.h"

#include "display.h"
#include "stdio.h"

#include "scroller.h"

void lcd_send_message_task()
{
    lcd_init();
    struct lcd_message message;
    
    vTaskDelay(pdMS_TO_TICKS(200));
    
    while (1)
    {
        if(message_queue != NULL)
        {
            if (xQueueReceive(message_queue, &message, 1) == pdPASS)
            {
                lcd_clear();
                lcd_cursor_set(message.xpos, message.ypos);
                lcd_write(message.text);
            }
        }
    }
}

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
#include "adc0values.h"

void lcd_send_message_task()
{
    lcd_init();
    struct lcd_message scroller_message;
    struct lcd_message adc0_value_message;
    
    vTaskDelay(pdMS_TO_TICKS(200));
    
    while (1)
    {
            if (xQueueReceive(scroller_queue, &scroller_message, 1) == pdPASS)
            {   
                lcd_cursor_set(scroller_message.xpos, scroller_message.ypos);
                lcd_write(scroller_message.text);
            }
            if (xQueueReceive(adc0_value_queue, &adc0_value_message, 1) == pdPASS)
            {               
                lcd_cursor_set(adc0_value_message.xpos, adc0_value_message.ypos);
                lcd_write(adc0_value_message.text);
            }
    }
}

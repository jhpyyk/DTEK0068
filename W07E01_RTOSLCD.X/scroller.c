#include "scroller.h"
#include "display.h"
#include "lcd.h"

#include <avr/io.h>
// FreeRTOS
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "stdio.h"
#include "string.h"

struct lcd_message message;
char scrolling_text[41];
void* ptr = scrolling_text;

void scroll_init(void)
{
    strcpy(scrolling_text, LOWER_LINE_TEXT);
    message.xpos = 1;
    message.ypos = 0;
    strncpy(message.text, ptr, LCD_DISPLAY_WIDTH);
    scroller_queue = xQueueCreate(1, sizeof(message));
    xQueueSend(
                scroller_queue,
                &message,
                1
               );
}

void scroll_left(void)
{
    ptr++;
    strncpy(message.text, ptr, LCD_DISPLAY_WIDTH);
}

void scroll_right(void)
{
    ptr--;
    strncpy(message.text, ptr, LCD_DISPLAY_WIDTH);
}

void scroll_task()
{
    scroll_init();
    scroller_queue = xQueueCreate(1, sizeof(message));
    vTaskDelay(pdMS_TO_TICKS(200));
    
    while (1)
    {
        for (uint8_t i = 0; i <= (strlen(scrolling_text) - LCD_DISPLAY_WIDTH - 1); i++)
        {
            scroll_left();
            xQueueSend(
                        scroller_queue,
                        &message,
                        1
                      );
            vTaskDelay(pdMS_TO_TICKS(200));
        }
        for (uint8_t i = 0; i <= (strlen(scrolling_text) - LCD_DISPLAY_WIDTH - 1); i++)
        {
            scroll_right();
            xQueueSend(
                        scroller_queue,
                        &message,
                        1
                      );
            vTaskDelay(pdMS_TO_TICKS(200));
        }
    }
}
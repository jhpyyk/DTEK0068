/* 
 * File:   display.h
 * Author: juuso
 *
 * Created on December 7, 2021, 4:04 PM
 */

#ifndef DISPLAY_H
#define	DISPLAY_H

#ifdef	__cplusplus
extern "C" {
#endif

#define LOWER_LINE_TEXT         "DTEK0068 Embedded Microprocessor Systems"
#define LCD_DISPLAY_WIDTH       16 

#include <avr/io.h>
// FreeRTOS
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "queue.h"
    
    
struct lcd_message
{
    uint16_t xpos;
    uint16_t ypos;
    char text[16];
};

QueueHandle_t scroller_queue;
QueueHandle_t adc0_value_queue;

void display_init(void);
void lcd_send_message_task();


#ifdef	__cplusplus
}
#endif

#endif	/* DISPLAY_H */


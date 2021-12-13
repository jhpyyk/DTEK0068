/* 
 * File:   backlight.h
 * Author: juuso
 *
 * Created on December 7, 2021, 4:03 PM
 */

#ifndef BACKLIGHT_H
#define	BACKLIGHT_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#define TCB_ZERO_DUTY   (0x00FF)
    
#include <avr/io.h>
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "timers.h"

void port_init (void);
void tca0_init (void);
void tcb3_init (void);
void backlight_init(void);
void backlight_adjust(uint16_t);
void backlight_adjust_task();
void backlight_turn_off(TimerHandle_t backlight_timer);


#ifdef	__cplusplus
}
#endif

#endif	/* BACKLIGHT_H */


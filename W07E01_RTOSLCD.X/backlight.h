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

#define TCB_CMP_VALUE   (0x80FF)

// Initialization functions
void port_init (void);
void tca0_init (void);
void tcb3_init (void);
void backlight_init(void);
void backlight_adjust(void);
void backlight_adjust_task();


#ifdef	__cplusplus
}
#endif

#endif	/* BACKLIGHT_H */


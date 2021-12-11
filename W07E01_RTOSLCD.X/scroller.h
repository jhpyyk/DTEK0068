/* 
 * File:   scroller.h
 * Author: juuso
 *
 * Created on December 7, 2021, 4:04 PM
 */

#ifndef SCROLLER_H
#define	SCROLLER_H

#ifdef	__cplusplus
extern "C" {
#endif

#define LOWER_LINE_TEXT         "DTEK0068 Embedded Microprocessor Systems"
#define LCD_DISPLAY_WIDTH       16
    
void scroll_init(void);
void scroll_left(void);
void scroll_right(void);
void scroll_task();

#ifdef	__cplusplus
}
#endif

#endif	/* SCROLLER_H */


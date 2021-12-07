/* 
 * File:   uart.h
 * Author: juuso
 *
 * Created on December 7, 2021, 4:05 PM
 */

#ifndef UART_H
#define	UART_H

#ifdef	__cplusplus
extern "C" {
#endif

// CPU clock frequency 3.33MHz
#define F_CPU                       3333333

// Copied from Microchip's TB3216.
// Used for calculating baud rate for USART.
#define USART1_BAUD_RATE(BAUD_RATE) \
((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

// Initialize USART0    
void usart0_init(void);

// Send character to the serial terminal via USART0
void usart0_send_char(uint8_t c);

// Send string to the serial terminal via USART0
void usart0_send_string(char *str);

// Send string to the serial terminal via USART0
// in one second intervals
void usart0_send_string_task();


#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */


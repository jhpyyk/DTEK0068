/*
 * File:   main.c
 * Author: Juuso Pyykkönen
 * 
 * Exercise for a university course, DTEK0068
 * 
 * Week 2 exercise 1, W02E01
 * 
 * Program makes a 7-segment display count down from 9 to 0
 * in 1 second intervals. If the red wire connected to PA4
 * is cut, the timer stops.
 * 
 *
 * Created on November 4, 2021, 7:15 PM
 */

#define F_CPU 3333333           // 3.33 MHz Clock frequency

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// g_running is 1 when numbers are counting  down
// and 0 when countdown is halted
volatile uint8_t g_running = 1;

int main(void) {
    
    // Set all pins in PORTC as outputs
    PORTC.DIRSET = (PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm
                    | PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm);
    
    // Set PA4 as input
    PORTA.DIRCLR = PIN4_bm;
    
    // Trigger interrupt when PA4 is disconnected
    PORTA.PIN4CTRL = PORT_ISC_FALLING_gc;
    
    
    // Numbers for the display, 0 to 9
    uint8_t seven_segment_numbers[] =
    {
        0b00111111, 0b00000110, 0b01011011, 
        0b01001111, 0b01100110, 0b01101101, 
        0b01111101, 0b00000111, 0b01111111, 
        0b01100111
    };
    
    // Countdown starts from 9
    int8_t number = 9;
    
    // Enable interrupts
    sei();
    
    while (1)
    {
        // Infinite loop to halt the program when g_running is 0
        if (!g_running)
        {
            while(1)
            {
            }
        }
        
        if (number < 0)
        {
            // Disable interrupts until the whole number is shown
            cli();

            // Toggle bits in display
            PORTC.OUTTGL = seven_segment_numbers[0];
            sei();
            _delay_ms(500);
        }
        else
        {
            // Disable interrupts until the whole number is shown
            cli();

            // Set number on display
            PORTC.OUT = seven_segment_numbers[number];
            sei();
            _delay_ms(1000);
            number--;
        }
    }
}
ISR(PORTA_PORT_vect)
{
    // Set g_running to 0 to halt the countdown
    g_running = 0;
    
    // Clear interrupts
    PORTA.INTFLAGS = 0xff;
}

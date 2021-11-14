/*
 * File:   main.c
 * Author: Juuso Pyykkönen
 * 
 * Exercise for a university course, DTEK0068
 * 
 * Week 3 exercise 1, W03E01
 * 
 * Program makes a 7-segment display count down from 9 to 0
 * in 1 second intervals. When timer reaches zero it starts
 * blinking. If the red wire connected to PA4
 * is cut, the timer stops.
 * 
 * Modified version from week 2. Updated to not use delays
 * and use RTC and interrupts instead. A transistor was added
 * to implement blinking with it.
 * 
 * The wiring of the display is the same as week 2
 * even though it differs from the wiring in this week's
 * exercise document. Seemed logical to use the already
 * working wiring.
 * 
 * Created on November 10, 2021, 5:38 PM
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include <avr/sleep.h>

// Numbers for the display, 0 to 9
const uint8_t seven_segment_digits[] =
{
    0b00111111,     // 0
    0b00000110,
    0b01011011, 
    0b01001111,
    0b01100110,
    0b01101101,     // 5
    0b01111101,
    0b00000111,
    0b01111111, 
    0b01100111      // 9
};

// 1 when numbers are counting down
// and 0 when countdown is halted
volatile uint8_t g_running = 1;

// PIT counts clockticks for the superloop
volatile uint8_t g_clockticks = 0;

// Function for initializing real-time counter.
// Copied from Microchip's documentation TB3213
// and slightly modified
void rtc_init(void);

// Function to setup pin configuration
void pin_setup(void);

int main(void)
{    
    // Enable sleep and set sleep mode to "idle"
    SLPCTRL.CTRLA |= SLPCTRL_SEN_bm;
    set_sleep_mode(SLPCTRL_SMODE_IDLE_gc);
    
    // Countdown starts from 9
    int8_t seconds = 9;
    
    // Pin configuration
    pin_setup();
    
    // Initialize real-time counter
    rtc_init();
    
    // Enable interrupts
    sei();
    
    while (1)
    {        
        // Infinite loop to halt the program when g_running is 0
        if (!g_running)
        {
            while(1)
            {
                ;
            }
        }
        
        // Update the digit in display every 8 clockticks
        // which is a 1 second interval.
        if (seconds >= 0 && g_clockticks == 8)
        {
            PORTC.OUT = seven_segment_digits[seconds];
            seconds--;
            g_clockticks = 0;
        }
        
        // Toggle the display on and off when countdown has reached 0
        // in half second intervals.
        if (seconds < 0 && g_clockticks == 4)
        {
            PORTF.OUTTGL = PIN5_bm;
            g_clockticks = 0;
        }
        sleep_mode();
    }
}

// Interrupt service routine for when PA4
// is disconnected. Halts countdown.
ISR(PORTA_PORT_vect)
{
    // Clear interrupt
    PORTA.INTFLAGS = PIN4_bm;
    
    // Set g_running to 0 to halt the countdown
    g_running = 0;
}

// Periodic interrupt which triggers 8 times per second.
// Counts clockticks for the superloop.
ISR(RTC_PIT_vect)
{
    // Clear flag by writing '1'
    RTC.PITINTFLAGS = RTC_PI_bm;
    g_clockticks++;
}

// Function to setup pin configuration
void pin_setup(void)
{
    // Set all pins in PORTC as outputs
    PORTC.DIRSET = 0xFF;
    
    // Set PA4 as input
    PORTA.DIRCLR = PIN4_bm;
    
    // Enable internal pull-up-resistor
    // and trigger interrupt when PA4 is disconnected
    PORTA.PIN4CTRL = PORT_PULLUPEN_bm | PORT_ISC_RISING_gc;
    
    // Set PF5 as output and set it to 1
    PORTF.DIRSET |= PIN5_bm;
    PORTF.OUTSET |= PIN5_bm;
}

// Function for initializing real-time counter.
// Copied from Microchip's documentation TB3213
// and slightly modified
void rtc_init(void)
{
    uint8_t temp;
    
    /* Initialize 32.768kHz Oscillator: */
    /* Disable oscillator: */
    temp = CLKCTRL.XOSC32KCTRLA;
    temp &= ~CLKCTRL_ENABLE_bm;
    /* Writing to protected register */
    ccp_write_io((void*)&CLKCTRL.XOSC32KCTRLA, temp);
    
    while(CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm)
    {
        ; /* Wait until XOSC32KS becomes 0 */
    }
    
    /* SEL = 0 (Use External Crystal): */
    temp = CLKCTRL.XOSC32KCTRLA;
    temp &= ~CLKCTRL_SEL_bm;
    /* Writing to protected register */
    ccp_write_io((void*)&CLKCTRL.XOSC32KCTRLA, temp);
    
    /* Enable oscillator: */
    temp = CLKCTRL.XOSC32KCTRLA;
    temp |= CLKCTRL_ENABLE_bm;
    /* Writing to protected register */
    ccp_write_io((void*)&CLKCTRL.XOSC32KCTRLA, temp);
    
    /* Initialize RTC: */
    while (RTC.STATUS > 0)
    {
        ; /* Wait for all register to be synchronized */
    }

    /* 32.768kHz External Crystal Oscillator (XOSC32K) */
    RTC.CLKSEL = RTC_CLKSEL_TOSC32K_gc;

    /* Run in debug: enabled */
    RTC.DBGCTRL = RTC_DBGRUN_bm;
    
    RTC.PITINTCTRL = RTC_PI_bm; /* Periodic Interrupt: enabled */
    
    // Set period to 4096 (1/8 second) and enable PIT function
    RTC.PITCTRLA = RTC_PERIOD_CYC4096_gc | RTC_PITEN_bm;
}

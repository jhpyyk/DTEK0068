/*
 * File:   main.c
 * Author: Juuso Pyykkönen
 * 
 * Exercise for a university course, DTEK0068
 * 
 * Week 4 exercise 1, W04E01
 * 
 * Used for playing the chrome dino game (chrome://dino/) with and LDR and a
 * servo with an arm to press the spacebar. The LDR reacts to light level
 * changes which are cacti in this context. Servo arm then presses the spacebar
 * to perform a jump. A potentiometer is used for adjusting the jump treshold
 * (light level when a cactus is "seen" by the LDR).
 * 
 * A 50ms time is given for both arm movements to complete which totals
 * 100ms for the whole jump move to happen.
 * 
 * 100ms is far too little time for the servo arm to fully perform the jump
 * but it used here because it is defined so in the functional specifications.
 * 
 * Created on November 16, 2021, 2:54 PM
 */

#include <avr/io.h>
#include <avr/cpufunc.h>
#include <avr/interrupt.h>


// Copied from course material
#define SERVO_PWM_PERIOD        (0x1046)
// Servo neutral position
#define SERVO_PWM_DUTY_NEUTRAL  (0x0138)
// Servo minimum position
#define SERVO_PWM_DUTY_MIN      (0x00D0)
// Servo maximum position
#define SERVO_PWM_DUTY_MAX      (0x01A0)

// Time for servo to move before it can move again ~50ms
#define RTC_MOVE_PERIOD         (1639)

// A more suitable period for movement time
// #define RTC_MOVE_PERIOD      (4095)

// Numbers for the display, 0x0 to 0xA
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
    0b01100111,
    0b01110111      // A
};

// Function to initialize pin configuration
void pin_init(void);

// Function for initializing real-time counter.
// Copied from Microchip's documentation TB3213
// and slightly modified
void rtc_init(void);

// Function to initialize ADC
void adc0_init(void);

// Function to initialize TCA
void tca0_init(void);

// Reads and returns ADC value
uint16_t adc0_read(void);

// Sets ADC0 to read from LDR
void adc0_set_ldr(void);

// Sets ADC to read from potentiometer
void adc0_set_potentiometer(void);

// Move servo to max position
void servo_move_max(void);

// Move servo to neutral position
void servo_move_neutral(void);

// Moves servo to max then neutral position
// which in this context is a jump
void jump(void);

// A jump can be performed when value 1
// Prevents continuous jumping.
uint8_t can_jump = 1;

// 1 when servo is moving.
// Prevents starting a move too fast.
volatile uint8_t g_servo_moving = 0;

int main(void)
{
    // Initializing functions
    pin_init();
    adc0_init();
    tca0_init();
    rtc_init();
    
    // Jump treshold value
    uint16_t treshold;
    
    // Digit for the display to show
    uint16_t digit;
    
    // Used for storing values read from LDR
    uint16_t ldr_value;
    
    // Enable interrupts
    sei();
    
    while (1)
    {
        // Set ADC0 to read values from potentiometer
        adc0_set_potentiometer();
        
        // Read the value from ADC0 and divide
        // it by 100 to display it in the
        // seven segment display
        treshold = adc0_read();
        digit = treshold/100;
        
        // Display digit in seven segment display
        PORTC.OUT = seven_segment_digits[digit];
        
        // Set AD0C to read values from LDR
        adc0_set_ldr();
        
        // Store read value
        ldr_value = adc0_read();
        
        // Perform a jump when LDR value exceeds treshold value
        // and jumping is allowed
        if ((ldr_value > treshold) && (can_jump))
        {
            jump();
        }
    }
}

// Moves servo to max then neutral position
// which in this context is a jump.
void jump(void)
{
    can_jump = 0;
    servo_move_max();
    servo_move_neutral();
    can_jump = 1;
}

// Moves servo to max position.
// Waits for RTC_MOVE_PERIOD == ~50ms
void servo_move_max(void)
{
    g_servo_moving = 1;
    TCA0.SINGLE.CMP2BUF = SERVO_PWM_DUTY_MAX;
    RTC.PER = RTC_MOVE_PERIOD;
    while (g_servo_moving)
    {
        ;
    }

}

// Moves servo to neutral position.
// Waits for RTC_MOVE_PERIOD == ~50ms
void servo_move_neutral(void)
{
    g_servo_moving = 1;
    TCA0.SINGLE.CMP2BUF = SERVO_PWM_DUTY_NEUTRAL;
    RTC.PER = RTC_MOVE_PERIOD;
    while (g_servo_moving)
    {
        ;
    }
}

// Function to initialize TCA0.
// Partly copied from course material.
void tca0_init(void)
{
    // Set TCA0 PWM waveform output on port B
    PORTMUX.TCAROUTEA = PORTMUX_TCA0_PORTB_gc;
    
    // Set TCA0 prescaler value to 16 (~208 kHz)
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV16_gc;
    
    // Set single-slop PWM generation mode
    TCA0.SINGLE.CTRLB |= TCA_SINGLE_WGMODE_SINGLESLOPE_gc;
    
    // Using double-buffering, set PWM period (20 ms)
    TCA0.SINGLE.PERBUF = SERVO_PWM_PERIOD;
    
    // Set initial servo arm position as neutral (0 deg)
    TCA0.SINGLE.CMP2BUF = SERVO_PWM_DUTY_NEUTRAL;
    
    // Enable Compare Channel 2
    TCA0.SINGLE.CTRLB |= TCA_SINGLE_CMP2EN_bm;
    
    // Enable TCA0 peripheral
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
}

// Sets ADC to read from LDR and changes the reference voltage
// to internal voltage 2.5V
void adc0_set_ldr(void)
{
    ADC0.CTRLC &= ~(ADC_REFSEL_VDDREF_gc);
    ADC0.CTRLC |= ADC_REFSEL_INTREF_gc;
    ADC0.MUXPOS = ADC_MUXPOS_AIN8_gc;
}

// Sets ADC to read from potentiometer and changes
// the reference voltage to VDD
void adc0_set_potentiometer(void)
{
    ADC0.CTRLC &= ~(ADC_REFSEL_INTREF_gc);
    ADC0.CTRLC |= ADC_REFSEL_VDDREF_gc;
    ADC0.MUXPOS = ADC_MUXPOS_AIN14_gc;
}

// Reads and returns ADC0 value
uint16_t adc0_read(void)
{
    ADC0.COMMAND = ADC_STCONV_bm;
        while(!(ADC0.INTFLAGS & ADC_RESRDY_bm))
        {
            ;
        }
        
        ADC0.INTFLAGS = ADC_RESRDY_bm;
        
        return ADC0.RES;
}

// Function to initialize pin configuration
void pin_init(void)
{
    // Set all pins in PORTC as outputs.
    // Seven segment display.
    PORTC.DIRSET = 0xFF;
    
    // Set PF5 as output and set it to 1.
    // Connected to the transistor.
    PORTF.DIRSET |= PIN5_bm;
    PORTF.OUTSET |= PIN5_bm;
    
    // Set PF4 as input and disable input buffer.
    // Conected to the potentiometer.
    PORTF.DIRCLR |= PIN4_bm;
    PORTF.PIN4CTRL = PORT_ISC_INPUT_DISABLE_gc;
    
    // Set PE0 as input and disable input buffer.
    // Connected to the LDR.
    PORTE.DIRCLR |= PIN0_bm;
    PORTE.PIN0CTRL = PORT_ISC_INPUT_DISABLE_gc;
    
    // Set 0-WO2 (PB2) as digital output.
    // Connected to the servo control.
    PORTB.DIRSET = PIN2_bm;
}

void adc0_init(void)
{
    // Divide CLK_PER by 16.   
    ADC0.CTRLC = ADC_PRESC_DIV16_gc;
    
    // Enable ADC and set resolution to 10.
    // Set internal reference to 2.5V.
    ADC0.CTRLA = ADC_ENABLE_bm
               | ADC_RESSEL_10BIT_gc
               | VREF_ADC0REFSEL_2V5_gc;
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

    // Set period to 50ms
    RTC.PER = RTC_MOVE_PERIOD;

    /* 32.768kHz External Crystal Oscillator (XOSC32K) */
    RTC.CLKSEL = RTC_CLKSEL_TOSC32K_gc;
    
       /* Run in debug: enabled */
    RTC.DBGCTRL |= RTC_DBGRUN_bm;
    
    // Enable RTC
    RTC.CTRLA = RTC_RTCEN_bm;
    
    /* Enable Overflow Interrupt */
    RTC.INTCTRL |= RTC_OVF_bm;
}

// Used as a timer for servo movement
ISR(RTC_CNT_vect)
{
    /* Clear flag by writing '1': */
    RTC.INTFLAGS = RTC_OVF_bm;
    while (RTC.STATUS > 0)
    {
        ;
    }
    g_servo_moving = 0;
}
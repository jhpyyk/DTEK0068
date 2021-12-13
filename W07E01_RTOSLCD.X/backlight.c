/* 
 * File:   backlight.c
 * Author: Juuso Pyykk�nen
 *
 * Sets LCD backlight brightness relative to the LDR.
 * Backlight turns off after 10 seconds without interaction
 * with the potentiometer.
 * 
 * Potentiometer change has a -+50 tolerance.
 * 
 * TCA is used for copying it to TCB, because TCB
 * does not have great enough prescaler.
 * 
 * Created on December 7, 2021, 4:03 PM
 */

#include <avr/io.h>
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "timers.h"
#include "backlight.h"
#include "adc.h"
#include "uart.h"
#include "task.h"
#include "semphr.h"
#include "lcd.h"

// Port, TCA and TCB inititialization functions are copied from
// Microchip's TB3214 and modified

TimerHandle_t backlight_timer;
uint8_t backlight_is_on = 0;
uint16_t last_pot_value;

void port_init(void)
{
    // Set PB5 as output and set it to low
    PORTB.DIRCLR = PIN5_bm;
    PORTB.OUTSET = PIN5_bm;
}

void tca0_init(void)
{
    // Set TCA0 prescaler value to 1024 (~3255Hz)
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1024_gc;
    
    // Enable TCA0 peripheral
    TCA0.SINGLE.CTRLA = TCA_SINGLE_ENABLE_bm;
}

void tcb3_init(void)
{
    
    // Zero duty cycle
    TCB3.CCMP = TCB_ZERO_DUTY;

    // Enable TCB3 and use clock from TCA
    TCB3.CTRLA |= TCB_ENABLE_bm;
    TCB3.CTRLA |= TCB_CLKSEL_CLKTCA_gc;
    
    /* Enable Pin Output and configure TCB in 8-bit PWM mode */
    TCB3.CTRLB |= TCB_CCMPEN_bm;
    TCB3.CTRLB |= TCB_CNTMODE_PWM8_gc;
}

void backlight_init(void)
{
    port_init();
    tca0_init();
    tcb3_init();
    
    // Timer to turn backlight off
    backlight_timer = xTimerCreate("backlight_timer",
                                   pdMS_TO_TICKS(10000),
                                   pdFALSE,
                                   (void *) 0,
                                   backlight_turn_off);
    backlight_adjust(ldr_read());
}

// value is a 10-bit value from ADC.
// Shifting it six bits preserves 8 most significant bits.
// CCMP register controls PWM for screen brightness.
void backlight_adjust(uint16_t value)
{
    TCB3.CCMP = 0x00FF | (value << 6);
}

void backlight_adjust_task()
{
    uint16_t new_pot_value = 0;
    backlight_init();
    
    vTaskDelay(pdMS_TO_TICKS(200));
    xTimerStart(backlight_timer, 0);
    
    while (1)
    {
        if (backlight_is_on)
        {
            backlight_adjust(ldr_read());
        }

        new_pot_value = potentiometer_read();

        if ((new_pot_value < (last_pot_value - 50)) ||
            (new_pot_value > (last_pot_value + 50)))
        {
            backlight_is_on = 1;
            last_pot_value = new_pot_value;
            xTimerReset(backlight_timer, 1);
        }
        vTaskDelay(0);
    }
    
    vTaskDelete(NULL);
}

// Sets backlight PWM to zero duty
void backlight_turn_off(TimerHandle_t backlight_timer)
{
    backlight_is_on = 0;
    backlight_adjust(0);
    xTimerStop(backlight_timer, 1);
}
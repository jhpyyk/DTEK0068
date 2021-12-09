#include "backlight.h"
#include <avr/io.h>
#include "adc.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"


#include "uart.h"

// All three inititialization functions are copied from
// Microchip's TB3214 and modified

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
    
    /* Load CCMP register with the period and duty cycle of the PWM */
    TCB3.CCMP = TCB_CMP_VALUE;

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
}

void backlight_adjust(void)
{
    TCB3.CCMP = 0x00FF | (ldr_read() << 8);
}

void backlight_adjust_task()
{
    vTaskDelay(pdMS_TO_TICKS(200));
    
    while (1)
    {
        backlight_adjust();
    }
    
    vTaskDelete(NULL);
}
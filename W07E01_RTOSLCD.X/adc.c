#include <avr/io.h>
#include "adc.h"

// Initialize ADC0
void adc0_init(void)
{
    // Set VREF source of 2.5 V (used for LDR conversions)
    VREF.CTRLA |= VREF_ADC0REFSEL_2V5_gc;
    
    // 10-bit resolution
    ADC0.CTRLA |=  ADC_RESSEL_10BIT_gc;
    
    // Use Vdd as reference voltage & CLK_ADC = CLK_CPU / 16
    ADC0.CTRLC |= ADC_REFSEL_VDDREF_gc | ADC_PRESC_DIV16_gc;
    
    // Enable ADC0
    ADC0.CTRLA = ADC_ENABLE_bm;
    
    // Start conversion
    ADC0.COMMAND = ADC_STCONV_bm;
    
    // Wait until ready
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm))
    {
        ;
    }
}

// Reads and returns ADC0 value
uint16_t adc0_read(void)
{
    // Start conversion
    ADC0.COMMAND = ADC_STCONV_bm;
    
    // Wait until ready
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm))
    {
        ;
    }

    return ADC0.RES;
}

// Read light-level from LDR (PE0)
uint16_t ldr_read()
{
    // Set reference to internal voltage (2.5V)
    ADC0.CTRLC = (ADC0.CTRLC & ~ADC_REFSEL_gm) | ADC_REFSEL_INTREF_gc;
    
    // Set ADC0 to read from PE0 (LDR)
    ADC0.MUXPOS = ADC_MUXPOS_AIN8_gc;
    
    // Discard first reading
    adc0_read();
    return adc0_read();
}

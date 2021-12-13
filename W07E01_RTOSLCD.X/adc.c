/* 
 * File:   adc.c
 * Author: Juuso Pyykkönen
 * 
 * Reads analog values from potentiometer, thermistor
 * and LDR. Converts them to digital to be used by tasks.
 *
 * Created on December 7, 2021, 4:04 PM
 */


#include <avr/io.h>
#include "adc.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"

// Mutex for controlled ADC reading between
// three components
SemaphoreHandle_t adc0_mutex;
uint16_t ldr_value = 0;
uint16_t pot_value = 0;
uint16_t thermistor_value = 0;


// Initialize ADC0
void adc0_init(void)
{
    // Set PF4 as input and disable input buffer.
    // Conected to the potentiometer.
    PORTF.DIRCLR = PIN4_bm;
    PORTF.PIN4CTRL = PORT_ISC_INPUT_DISABLE_gc;
    
    // Set PE0 and PE1 as inputs and disable input buffer.
    // PE0 is connected to the LDR.
    // PE1 is connected to the thermometer.
    PORTE.DIRCLR = PIN0_bm | PIN1_bm;
    PORTE.PIN0CTRL = PORT_ISC_INPUT_DISABLE_gc;
    PORTE.PIN1CTRL = PORT_ISC_INPUT_DISABLE_gc;
    
    // Set VREF source of 2.5 V (used for LDR conversions)
    VREF.CTRLA |= VREF_ADC0REFSEL_2V5_gc;
    
    // 10-bit resolution
    ADC0.CTRLA |=  ADC_RESSEL_10BIT_gc;
    
    // Use Vdd as reference voltage & CLK_ADC = CLK_CPU / 16
    ADC0.CTRLC |= ADC_REFSEL_VDDREF_gc | ADC_PRESC_DIV16_gc;
    
    // Set ADC0 to read from potentiometer (PF4)
    ADC0.MUXPOS = ADC_MUXPOS_AIN14_gc;
    
    // Enable ADC0
    ADC0.CTRLA = ADC_ENABLE_bm;
    
    // Start conversion
    ADC0.COMMAND = ADC_STCONV_bm;
    
    // Wait until ready
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm))
    {
        ;
    }
    
    // Mutex for controlled ADC reading between
    // three components
    adc0_mutex = xSemaphoreCreateMutex();
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
uint16_t ldr_read(void)
{
    // Prevent other ADC-reading functions from intervening
    if (xSemaphoreTake(adc0_mutex, 1) == pdTRUE)
    {
        // Set reference to internal voltage (2.5V)
        ADC0.CTRLC = (ADC0.CTRLC & ~ADC_REFSEL_gm) | ADC_REFSEL_INTREF_gc;

        // Set ADC0 to read from LDR (PE0)
        ADC0.MUXPOS = ADC_MUXPOS_AIN8_gc;

        // Discard first reading
        adc0_read();
        ldr_value = adc0_read();
        
        // Enable other ADC-reading functions
        xSemaphoreGive(adc0_mutex);
    }
    return ldr_value;
}

// Read potentiometer value (PF4)
uint16_t potentiometer_read(void)
{
    // Prevent other ADC-reading functions from intervening
    if (xSemaphoreTake(adc0_mutex, 1) == pdTRUE)
    {
        // Set reference voltage to Vdd
        ADC0.CTRLC = (ADC0.CTRLC & ~ADC_REFSEL_gm) | ADC_REFSEL_VDDREF_gc;

        // Set ADC0 to read from potentiometer (PF4)
        ADC0.MUXPOS = ADC_MUXPOS_AIN14_gc;

        // Discard first reading
        adc0_read();
        pot_value = adc0_read();
        
        // Enable other ADC-reading functions
        xSemaphoreGive(adc0_mutex);
    }
    return pot_value;
}

// Read thermistor value (PE1)
uint16_t thermistor_read(void)
{
    // Prevent other ADC-reading functions from intervening
    if (xSemaphoreTake(adc0_mutex, 1) == pdTRUE)
    {
        // Set reference voltage to Vdd
        ADC0.CTRLC = (ADC0.CTRLC & ~ADC_REFSEL_gm) | ADC_REFSEL_VDDREF_gc;

        // Set ADC0 to read from thermistor (PE1)
        ADC0.MUXPOS = ADC_MUXPOS_AIN9_gc;

        // Discard first reading
        adc0_read();
        thermistor_value = adc0_read();
        
        // Enable other ADC-reading functions
        xSemaphoreGive(adc0_mutex);
    }
    return thermistor_value;
}

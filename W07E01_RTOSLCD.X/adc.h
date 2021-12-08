/* 
 * File:   adc.h
 * Author: juuso
 *
 * Created on December 7, 2021, 4:03 PM
 */

#ifndef ADC_H
#define	ADC_H

#ifdef	__cplusplus
extern "C" {
#endif

// Initialize ADC0
void adc0_init(void);

// Reads and returns ADC0 value
uint16_t adc0_read(void);

// Read light-level from LDR (PE0)
uint16_t ldr_read();

// Read potentiometer value (PF4)
uint16_t potentiometer_read();

// Read thermistor value (PE1)
uint16_t thermistor_read();


#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */


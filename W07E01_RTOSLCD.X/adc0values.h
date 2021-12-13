/* 
 * File:   adc0values.h
 * Author: juuso
 *
 * Created on December 12, 2021, 2:35 PM
 */

#ifndef ADC0VALUES_H
#define	ADC0VALUES_H

#ifdef	__cplusplus
extern "C" {
#endif

void adc0_value_init(void);

// First reads LDR value and sends it as a message to LCD.
// Then thermistor value.
// Then potentiometer.
// Starts over.
// Messages are sent in ~660ms intervals.
void adc0_value_task();


#ifdef	__cplusplus
}
#endif

#endif	/* ADC0VALUES_H */


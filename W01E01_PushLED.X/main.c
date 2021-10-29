/*
 * File:   main.c
 * Author: Juuso Pyykkonen
 *
 * Created on October 29, 2021, 9:01 PM
 */


#include <avr/io.h>

int main(void) {
    
    PORTF.DIR |= PIN5_bm;
    PORTF.DIR &= ~PIN6_bm;
    
    while (1)
    {
        if(PORTF.IN & PIN6_bm)
        {
            PORTF.OUT |= PIN5_bm;
        }
        else
        {
            PORTF.OUT &= ~PIN5_bm; 
        }
    }
}

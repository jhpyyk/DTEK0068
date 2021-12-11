#include <avr/io.h>
#include "uart.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "clock_config.h"
#include "task.h"
#include "string.h"
#include "stdio.h"

#include "adc.h"

// Initialize USART0
void usart0_init(void)
{
    // Set PA0 to output (transmit)
    PORTA.DIRSET = PIN0_bm;
    
    // Set baud rate to 9600
    USART0.BAUD = (uint16_t)USART1_BAUD_RATE(9600);
    
    // Enable USART0 transmitting
    USART0.CTRLB = USART_TXEN_bm;
}

// Send character to the serial terminal via USART0
void usart0_send_char(uint8_t c)
{
    while (!(USART0.STATUS & USART_DREIF_bm))
    {
        ;
    }
    USART0.TXDATAL = c;
}

// Send string to the serial terminal via USART0
void usart0_send_string(char *str)
{
    for (uint8_t i = 0; i < strlen(str); i++)
    {
        usart0_send_char(str[i]);
    }
}
// Send string to the serial terminal via USART0
// in one second intervals 
void usart0_send_string_task()
{
    usart0_init();
    vTaskDelay(pdMS_TO_TICKS(200));
    while (1)
    {
        uint16_t ldr = ldr_read();
        char str[32];
        sprintf(str, "LDR value: %d\r\n", ldr);
        usart0_send_string(str);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    vTaskDelete(NULL);
}
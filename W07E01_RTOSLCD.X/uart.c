#include <avr/io.h>
#include "uart.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "clock_config.h"
#include "task.h"
#include "string.h"

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
    vTaskDelay(pdMS_TO_TICKS(200));
    while (1)
    {
        usart0_send_string("ADC value: 1023\r\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    vTaskDelete(NULL);
}
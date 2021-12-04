/*
 * File:   main.c
 * Author: Juuso Pyykkönen
 *
 * Created on December 3, 2021, 4:00 PM
 */

#define F_CPU 3333333
#define USART1_BAUD_RATE(BAUD_RATE) \
((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

#include <avr/io.h>
#include <string.h>
#include "FreeRTOS.h"
#include "clock_config.h"
#include "task.h"
#include "queue.h"
#include <stdio.h>

// Queue for display numbers
QueueHandle_t g_number_queue;

// Queue for characters sent to USART
QueueHandle_t g_character_queue;

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
    0b01100111,     // 9
    0b11111001      // E.
};

// Initialize seven segment display
void display_init(void)
{
    // Set all pins in PORTC as outputs.
    // Seven segment display.
    PORTC.DIRSET = 0xFF;
    
    // Set PF5 as output and set it to 1.
    // Connected to the transistor.
    PORTF.DIRSET = PIN5_bm;
    PORTF.OUTSET = PIN5_bm;
}

// Initialize USART
void usart_init(void)
{
    // Set PA0 to output (transmit)
    // and PA1 to input (receive)
    PORTA.DIRSET = PIN0_bm;
    PORTA.DIRCLR = PIN1_bm;
    
    // Set baud rate to 9600
    USART0.BAUD = (uint16_t)USART1_BAUD_RATE(9600);
    
    // Enable USART0 transmitting and receiving
    USART0.CTRLB |= USART_TXEN_bm;
    USART0.CTRLB |= USART_RXEN_bm;
}

// Send characters to USART0
void usart0_send_char(char c)
{
    while (!(USART0.STATUS & USART_DREIF_bm))
    {
        ;
    }
    USART0.TXDATAL = c;
}

void usart0_send_string(char *str)
{
    for (uint8_t i = 0; i < strlen(str); i++)
    {
        usart0_send_char(str[i]);
    }
}

char usart0_read_char(void)
{
    while (!(USART0.STATUS & USART_RXCIF_bm))
    {
        ;
    }
    return USART0.RXDATAL;
}

void send_number_to_queue(uint8_t number)
{
    xQueueSend(
                g_number_queue,
                (void *) &number,
                pdMS_TO_TICKS(10)
              );
}

void received_character_handle(char c)
{
    uint8_t c_ascii_value = (uint8_t)c;
    
    if ((c_ascii_value >= 48) && (c_ascii_value <= 57))
    {
        char str[4] = {c_ascii_value, '\r','\n'};
        usart0_send_string(str);
        send_number_to_queue(c_ascii_value - 48);
    }
    else if (c_ascii_value)
    {
        usart0_send_string("Error! Not a valid digit.\r\n");
        send_number_to_queue(10);
    }
}
// Send characters to USART0
void send_char_to_queue_task(void)
{
    char c;
    while (1)
    {
        c = usart0_read_char();
        if (c != '\0')
        {
            xQueueSend(
                        g_character_queue,
                        (void *) &c,
                        pdMS_TO_TICKS(10)
                      );
            c = '\0';
        }
    }
    vTaskDelete(NULL);
}


void receive_char_from_queue_task(void)
{
    char c;
    while (1)
    {
        c = '\0';
        if (g_character_queue != NULL)
        {
            xQueueReceive(
                            g_character_queue,
                            &(c),
                            pdMS_TO_TICKS(10)
                         );
            received_character_handle(c);
        }
    }
    vTaskDelete(NULL);
}

// Task to display numbers from 0x0 to 0xA with 250ms delay
// and then repeat indefinitely
void display_number_task(void)
{
    uint8_t number = 10;
    
    while (1)
    {
        if (g_number_queue != NULL)
        {
            xQueueReceive(
                            g_number_queue,
                            &(number),
                            pdMS_TO_TICKS(10)
                         );
        
            PORTC.OUT = seven_segment_digits[number];
        }
    }
    vTaskDelete(NULL);
}

void iterate_numbers_task(void* parameter)
{
    uint8_t i = 0;
    while (1)
    {
        xQueueSend(
                    g_number_queue,
                    (void *) &i,
                    pdMS_TO_TICKS(10)
                  );
        i++;
        if (i == 10)
        {
            i = 0;
        }
        vTaskDelay(pdMS_TO_TICKS(250));
    }
    vTaskDelete(NULL);
}

int main(void)
{
    display_init();
    usart_init();
    
    g_number_queue = xQueueCreate(10, sizeof(uint8_t));
    g_character_queue = xQueueCreate(10, sizeof(char));
    
    // Create task for displaying numbers
    xTaskCreate(
                display_number_task,
                "display_number",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY,
                NULL
               );
    /*xTaskCreate(
                iterate_numbers_task,
                "iterate_numbers",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY,
                NULL
               );*/
    xTaskCreate(
                send_char_to_queue_task,
                "send_character_to_queue",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY,
                NULL
               );
    xTaskCreate(
                receive_char_from_queue_task,
                "receive_character_from_queue",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY,
                NULL
               );
    
    // Start the scheduler
    vTaskStartScheduler();
    
    // Scheduler will not return
    return 0;
}

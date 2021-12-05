/*
 * File:   main.c
 * Author: Juuso Pyykkönen
 * 
 * Exercise for a university course, DTEK0068
 * 
 * Week 6 exercise 1, W06E01
 * 
 * A seven segment display shows a digit when a digit is pressed on
 * the host keyboard (when in serial terminal). When (almost) any other key
 * is pressed, the display shows an error character E, with a dot (E.) to
 * distinguish it from number 3. The digit or an error message is also
 * printed in the serial terminal when a key is pressed.
 * 
 * Multi-byte character's 2nd, 3rd and 4th byte could be tested and ignored
 * with bitwise operation, since their binary code starts with 10. Multi-byte
 * characters are checked so that only one error is printed.
 * When F1 - F12, HOME, END and some other keys are pressed, many lines are
 * printed. The test for multi-byte characters does not work on them because
 * they do not follow UTF-8 rules. I tested the keys with showkey and for
 * example HOME-keys first byte is 00011011.
 * 
 * The program uses FreeRTOS, and USART to communicate with the host serial
 * terminal.
 *
 * Created on December 3, 2021, 4:00 PM
 */

// CPU clock frequency 3.33MHz
#define F_CPU                       3333333

// Copied from Microchip's TB3216.
// Used for calculating baud rate for USART.
#define USART1_BAUD_RATE(BAUD_RATE) \
((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

// Error character's index in seven_segment_digits
#define ERROR_CHARACTER_INDEX       10

// ASCII digit values.
// 48 = '0', 57 = '9'
#define ASCII_DIGIT_START           48
#define ASCII_DIGIT_END             57

#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "clock_config.h"
#include "task.h"
#include "queue.h"

// Queue for numbers to show on display
volatile QueueHandle_t g_digit_queue;

// Queue for characters to be sent via USART
volatile QueueHandle_t g_character_queue;

// Digits and an error character for the seven segment display
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

// Initialize USART0
void usart0_init(void)
{
    // Set PA0 to output (transmit)
    // and PA1 to input (receive)
    PORTA.DIRSET = PIN0_bm;
    PORTA.DIRCLR = PIN1_bm;
    
    // Set baud rate to 9600
    USART0.BAUD = (uint16_t)USART1_BAUD_RATE(9600);
    
    // Enable USART0 transmitting and receiving
    USART0.CTRLB = USART_TXEN_bm
                 | USART_RXEN_bm;
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

// Receive character from serial terminal via USART0
uint8_t usart0_receive_char(void)
{
    while (!(USART0.STATUS & USART_RXCIF_bm))
    {
        ;
    }
    return USART0.RXDATAL;
}

// Send digit to digit queue to be displayed in the
// seven segment display
void send_digit_to_queue(uint8_t digit)
{
    xQueueSend(
                g_digit_queue,
                (void *) &digit,
                pdMS_TO_TICKS(10)
              );
}

// Sends received character to digit queue and to the serial terminal
// if it is a digit. If it's any other character, the error character is
// sent to digit queue and an error message is sent to the serial terminal.
void received_character_handle(uint8_t c)
{
    // Check if character is 2nd, 3rd or 4th byte of a multi-byte character
    // and ignore it
    if ((c & (1 << 7)) && !(c & (1 << 6)))
    {
        ;
    }
    else if ((c >= ASCII_DIGIT_START) &&
        (c <= ASCII_DIGIT_END))
    {
        char str[4] = {c, '\r','\n'};
        usart0_send_string(str);
        send_digit_to_queue(c - ASCII_DIGIT_START);
    }
    else if (c)
    {
        usart0_send_string("Error! Not a valid digit.\r\n");
        send_digit_to_queue(ERROR_CHARACTER_INDEX);
    }
}
// Receive a character from the serial terminal and send it to queue
void send_char_to_queue_task()
{
    uint8_t c = 0;
    while (1)
    {
        c = usart0_receive_char();
        if (c)
        {
            xQueueSend(
                        g_character_queue,
                        (void *) &c,
                        pdMS_TO_TICKS(10)
                      );
            c = 0;
        }
    }
    vTaskDelete(NULL);
}

// Receive a character from queue and pass it to the handle function
void receive_char_from_queue_task()
{
    uint8_t c = 0;
    while (1)
    {
        if (g_character_queue != NULL)
        {
            xQueueReceive(
                            g_character_queue,
                            &(c),
                            pdMS_TO_TICKS(10)
                         );
            received_character_handle(c);
            c = 0;
        }
    }
    vTaskDelete(NULL);
}

// Receive a digit from digit queue and display
// it in the seven segment display
void display_digit_task()
{
    uint8_t digit = ERROR_CHARACTER_INDEX;
    
    while (1)
    {
        if (g_digit_queue != NULL)
        {
            xQueueReceive(
                            g_digit_queue,
                            &(digit),
                            pdMS_TO_TICKS(10)
                         );
        
            PORTC.OUT = seven_segment_digits[digit];
        }
    }
    vTaskDelete(NULL);
}

// Calls initialization functions, creates the queues and the tasks then
// starts the scheduler
int main(void)
{
    display_init();
    usart0_init();
    
    // Queue length 8 is arbitrary
    g_digit_queue = xQueueCreate(8, sizeof(uint8_t));
    g_character_queue = xQueueCreate(8, sizeof(uint8_t));
    
    // Create a task for displaying numbers
    xTaskCreate(
                display_digit_task,
                "display_digit",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY,
                NULL
               );
    
    // Create a task to send characters to the character queue
    xTaskCreate(
                send_char_to_queue_task,
                "send_char_to_queue",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY,
                NULL
               );
    
    // Create a task to receive characters from the character queue
    xTaskCreate(
                receive_char_from_queue_task,
                "receive_char_from_queue",
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
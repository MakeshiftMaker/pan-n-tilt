/**
 * @Authors: Mic Lab Team
 */

#include "usart.h"
#include "utils/bit.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/atomic.h>
#include <util/delay.h>

#define TX_BUF_SIZE 255
#define RX_BUF_SIZE 64

static volatile uint8_t rxBuffer[RX_BUF_SIZE] = {0};
static volatile uint8_t rxHead = 0;
static volatile uint8_t rxTail = 0;

static uint8_t txBuffer[TX_BUF_SIZE] = {0};
static volatile uint8_t txLength = 0;
static volatile uint8_t txPos = 0;

static inline void enableTransmitBufferEmptyInterrupt() /* set UDRIE 1= enable data Register empty interrupt */
{
    BIT_SET(UCSR0B, UDRIE0);
}

static inline void disableTransmitBufferEmptyInterrupt() /* set UDRIE 0= disable data Register empty interrupt */
{
    BIT_CLR(UCSR0B, UDRIE0);
}

void usartResetTransmission()
{
    disableTransmitBufferEmptyInterrupt();
    txLength = 0;
    txPos = 0;
}

ISR(USART_UDRE_vect)
{
    if (txPos < txLength)
    {
        UDR0 = txBuffer[txPos];
        txPos++;
    }
    else
    {
        usartResetTransmission();
    }
}

void usartSetup(UsartBaudrate baud, UsartConfig config)
{
    if (config != USART_CONFIG_8N1)
    {
        return; /* Unsupported */
    }
    //pre-transition :: BIT(URSEL) | 
    UCSR0C = BIT(UCSZ00) | BIT(UCSZ01);

    uint16_t ubrrValue = (F_CPU / (16UL * baud)) - 1;
    UBRR0L = ubrrValue & 0xFF;
    UBRR0H = (ubrrValue << 8) & 0xFF;

    BIT_SET(UCSR0B, TXEN0);
    BIT_SET(UCSR0B, RXEN0);
    BIT_SET(UCSR0B, RXCIE0);
}

uint8_t usartWriteString(const char *str)
{
    if (txLength != 0)
    {
        return 0; /* busy */
    }
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        uint16_t i = 0;
        for (i = 0; i < TX_BUF_SIZE && str[i] != '\0'; i++)
        {

            txBuffer[i] = str[i];
        }
        txPos = 0;
        txLength = i;
        enableTransmitBufferEmptyInterrupt();
    }
    return txLength;
}

uint8_t usartPrint(const char *format, ...)
{
    const uint8_t bufferSize = 255;
    char buffer[bufferSize];

    /* Initialize buffer with zero-length string, in case vsnprintf() fails */
    buffer[0] = '\0';

    va_list args;
    va_start(args, format);
    vsnprintf(buffer, bufferSize, format, args);
    va_end(args);

    return usartWriteString(buffer);
}

bool usartReadLine(char *out, uint8_t maxLength)
{
    static char lineBuf[64];
    static uint8_t lineIndex = 0;

    while (rxTail != rxHead)
    {
        char c = rxBuffer[rxTail];
        rxTail = (rxTail + 1) % RX_BUF_SIZE;

        if (c == '\n' || c== '\r')  // Your chosen terminator
        {
            if (lineIndex > 0)
            {
                // Proper termination
                lineBuf[lineIndex] = '\0';
                
                // Ensure we don't overflow the output
                strncpy(out, lineBuf, maxLength - 1);
                out[maxLength - 1] = '\0';  // Always null-terminate

                lineIndex = 0; // reset for next line
                return true;
            }
            else
            {
                continue; // ignore empty lines
            }
        }
        else if (lineIndex < sizeof(lineBuf) - 1)
        {
            lineBuf[lineIndex++] = c;
        }
        // else: silently ignore overflow
    }

    return false;
}




ISR(USART_RX_vect)
{
    uint8_t data = UDR0;
    uint8_t nextHead = (rxHead + 1) % RX_BUF_SIZE;

    // Echo typed character (optional)
    //usartWriteString((char[]){data, '\0'});

    if (nextHead != rxTail)
    {
        rxBuffer[rxHead] = data;
        rxHead = nextHead;
    }
    // else: Buffer overflow, data lost
}

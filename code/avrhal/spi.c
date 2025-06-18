#include "avrhal/spi.h"
#include "utils/bit.h"
#include <avr/io.h>


void spiSetup()
{
    SPCR = 0;
    SPCR = BIT(SPE) | BIT(MSTR); //enable bit
    SPSR = 0;
    SPSR = BIT(SPI2X); //fclk/2

    //set input/output pins
    DDRB = 0;
    DDRB = BIT(PB5) | BIT(PB7);
}

uint8_t spiTransferByte(uint8_t data)
{
    /* Write data byte to register, initiating the transmission */
    /* Wait until the flag-bit is set, indicating a completed data byte transfer */
    /* Return the byte received from the slave IC */
    SPDR = data;
    while(!(SPSR & BIT(SPIF)));
    return SPDR;
}

#include "avrhal/adc.h"
#include "utils/bit.h"
#include <avr/io.h>

void adcSetup()
{
    ADMUX = BIT(REFS0);
    ADCSRA = BIT(ADEN) | BIT(ADPS2) | BIT(ADPS1) | BIT(ADPS0);
}

uint16_t adcRead()
{
    /* Start conversion and busy-wait for conversion complete */
    BIT_SET(ADCSRA, ADSC);
    while (BIT_IS_SET(ADCSRA, ADSC))
        ;

    return ADCW;
}

void adcSetChannel(__attribute__((unused)) uint8_t channel)
{
    /* Clear 5 lower bits for channel selection */
    ADMUX &= 0xE0;
    /* Set single ended channel */
    ADMUX |= channel & 0b111;
}

void adcSetupFreeRunning()
{
}

int16_t adcLastRead(__attribute__((unused)) uint8_t channel)
{
    return -1;
}

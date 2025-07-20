#include "adc.h"
#include "utils/bit.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/* Interner ADC-Puffer */
static volatile uint16_t adcResults[8];
static volatile uint8_t currentChannel = 0;

void adcSetup()
{
    DDRC &= ~((1 << PC0) | (1 << PC1));

    DDRC &= ~(1 << PC2);   // PA2 als Eingang
    PORTC |= (1 << PC2);
    /* Einzelmessung mit AREF als Referenzspannung */
    ADMUX = 0;  // REFS1=0, REFS0=0 → externe Referenz an AREF-Pin
    ADCSRA = BIT(ADEN)                 // ADC aktivieren
           | BIT(ADPS2) | BIT(ADPS1) | BIT(ADPS0); // Prescaler = 128
}

uint16_t adcRead()
{
    BIT_SET(ADCSRA, ADSC);                 // Messung starten
    while (BIT_IS_SET(ADCSRA, ADSC));      // Warten bis fertig
    return ADCW;                           
}

void adcSetChannel(uint8_t channel)
{
    /* Clear 5 lower bits for channel selection */
    ADMUX &= 0xE0;
    /* Set single ended channel */
    ADMUX |= channel & 0b111;
}

void adcSetupFreeRunning()
{
    currentChannel = 0;
    adcSetChannel(currentChannel);

    ADCSRA = BIT(ADEN)                 // ADC aktivieren
           | BIT(ADATE)                // Auto Trigger Enable
           | BIT(ADIE)                 // Interrupt Enable
           | BIT(ADPS2) | BIT(ADPS1) | BIT(ADPS0); // Prescaler 128

    ADCSRB &= ~(BIT(ADTS2) | BIT(ADTS1) | BIT(ADTS0)); // Free Running Mode

    BIT_SET(ADCSRA, ADSC);  // Erste Wandlung starten
}

int16_t adcLastRead(uint8_t channel)
{
    if (channel > 7) return -1;
    return adcResults[channel];
}

/* ADC Interrupt: bei jeder Wandlung ausgelöst */
ISR(ADC_vect)
{
    adcResults[currentChannel] = ADCW;

    /* Nächsten Kanal auswählen */
    currentChannel = (currentChannel == 0) ? 1 : 0;
    adcSetChannel(currentChannel);
}

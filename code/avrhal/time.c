#include "avrhal/time.h"
#include "utils/bit.h"
#include "avrhal/stepper.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>

static volatile uint32_t counter;


ISR(TIMER0_COMP_vect)
{
    counter++;
    //stepper_tick();
}

/** Enables timer 0 compare match interrupt. */
static inline void enableTimerInterrupt()
{
    BIT_SET(TIMSK, OCIE0);
}

/** Disables timer 0 compare match interrupt. */
static inline void disableTimerInterrupt()
{
    BIT_CLR(TIMSK, OCIE0);
}


void timeSetup()
{
    counter = 0;

    // CTC Mode (Clear Timer on Compare Match)
    TCCR0 = (1 << WGM01); // CTC mode
    OCR0 = 7;             // Compare match every 8 ticks = 1µs @ 8MHz
    TCCR0 |= (1 << CS00); // No prescaling (clk/1)

    enableTimerInterrupt(); // Enable OCIE0 interrupt
}

uint32_t timeUSec()
{
    uint32_t value;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        value = counter;
    }
    return value;
}

#include "stepper.h"
#include <util/delay.h>

#define STEP_DELAY_US 5000

void stepper_setup(Stepper* s)
{
    *(s->ddr) |= (1 << s->step_pin) | (1 << s->dir_pin) | (1 << s->en_pin) |
                 (1 << s->ms1_pin) | (1 << s->ms2_pin) | (1 << s->ms3_pin) |
                 (1 << s->rst_pin) | (1 << s->slp_pin);

    *(s->port) &= ~((1 << s->step_pin) | (1 << s->dir_pin));
    *(s->port) |= (1 << s->rst_pin) | (1 << s->slp_pin);
    *(s->port) &= ~(1 << s->en_pin);
    *(s->port) &= ~((1 << s->ms1_pin) | (1 << s->ms2_pin) | (1 << s->ms3_pin));

    _delay_us(5000);
}

void stepper_set_microstep_mode(Stepper* s, MicrostepMode mode)
{
    // Clear MS1–MS3 bits
    *(s->port) &= ~((1 << s->ms1_pin) | (1 << s->ms2_pin) | (1 << s->ms3_pin));

    // Set bits if they are 1 in the mode
    if (mode & 0b100) *(s->port) |= (1 << s->ms1_pin);
    if (mode & 0b010) *(s->port) |= (1 << s->ms2_pin);
    if (mode & 0b001) *(s->port) |= (1 << s->ms3_pin);

    _delay_us(1000); // optional: give driver time to react
}


void stepper_set_direction(Stepper* s, bool dir)
{
    if (dir)
        *(s->port) |= (1 << s->dir_pin);
    else
        *(s->port) &= ~(1 << s->dir_pin);

    _delay_us(5000);
}

void stepper_step(Stepper* s)
{
    *(s->port) |= (1 << s->step_pin);
    _delay_us(STEP_DELAY_US);
    *(s->port) &= ~(1 << s->step_pin);
    _delay_us(STEP_DELAY_US);
}

void stepper_step_n(Stepper* s, int steps)
{
    for (int i = 0; i < steps; i++) {
        stepper_step(s);
    }
}

#include "stepper.h"
#include "utils/bit.h"
//#include <util/delay.h>
#include <avr/interrupt.h>

//#define STEP_DELAY_US 5000
#define FCLK 8000000

void stepper_setup(Stepper *s)
{
    /*
    *(s->ddr) |= (1 << s->step_pin) | (1 << s->dir_pin) | (1 << s->en_pin) |
                 (1 << s->ms1_pin) | (1 << s->ms2_pin) | (1 << s->ms3_pin) |
                 (1 << s->rst_pin) | (1 << s->slp_pin);

    *(s->port) &= ~((1 << s->step_pin) | (1 << s->dir_pin));
    *(s->port) |= (1 << s->rst_pin) | (1 << s->slp_pin);
    *(s->port) &= ~(1 << s->en_pin);
    *(s->port) &= ~((1 << s->ms1_pin) | (1 << s->ms2_pin) | (1 << s->ms3_pin));
    */

    // Set direction of control pins as output
    BIT_SET(*(s->ddr), s->step_pin);
    BIT_SET(*(s->ddr), s->dir_pin);
    BIT_SET(*(s->ddr), s->en_pin);
    BIT_SET(*(s->ddr), s->ms1_pin);
    BIT_SET(*(s->ddr), s->ms2_pin);
    BIT_SET(*(s->ddr), s->ms3_pin);
    BIT_SET(*(s->ddr), s->rst_pin);
    BIT_SET(*(s->ddr), s->slp_pin);

    // Set STEP and DIR low
    BIT_CLR(*(s->port), s->step_pin);
    BIT_CLR(*(s->port), s->dir_pin);

    // Set RST and SLP high
    BIT_SET(*(s->port), s->rst_pin);
    BIT_SET(*(s->port), s->slp_pin);

    // Set EN (active-low) high = disable driver
    //default disabled
    BIT_SET(*(s->port), s->en_pin);

    // Clear microstep mode bits (full step mode)
    BIT_CLR(*(s->port), s->ms1_pin);
    BIT_CLR(*(s->port), s->ms2_pin);
    BIT_CLR(*(s->port), s->ms3_pin);

    //_delay_us(5000);
}

void stepper_heartbeat_setup(int f)
{   //set up timer1 to interrupt with frequency f Hz
    // set up the TCCR1A/B registries

    // clear Compare Output Modes for PB1/PB2 normal operation
    BIT_CLR(TCCR1A, COM1A1);
    BIT_CLR(TCCR1A, COM1A0);
    BIT_CLR(TCCR1A, COM1B1);
    BIT_CLR(TCCR1A, COM1B0);
    // clear force output compare as per datasheet
    BIT_CLR(TCCR1A, FOC1A);
    BIT_CLR(TCCR1A, FOC1B);
    // set WGM in TCCR1A/B to use CTC compare mode
    BIT_CLR(TCCR1A, WGM10);
    BIT_CLR(TCCR1A, WGM11);
    BIT_SET(TCCR1B, WGM12);
    BIT_CLR(TCCR1B, WGM13);
    // disable Input Capture noise canceler and clear Input Capture Edge Select
    BIT_CLR(TCCR1B, ICNC1);
    BIT_CLR(TCCR1B, ICES1);
    // set Clock Select bits to use prescaler 8, 8MHZ/8 is 1MHZ = 1us per tick
    // clock is disabled on setup, use heartbeat_enable to start
    /*
    BIT_CLR(TCCR1B, CS12);
    BIT_SET(TCCR1B, CS11);
    BIT_CLR(TCCR1B, CS10);
    */
    // enable interrupts on compare match
    BIT_SET(TIMSK, OCIE1A);

    //calculate OCR1A according to desired frequency
    OCR1A = (FCLK/(16*f))-1;
}

void stepper_heartbeat_enable()
{
    // Set prescaler to 8: CS12 = 0, CS11 = 1, CS10 = 0
    BIT_CLR(TCCR1B, CS12);
    BIT_SET(TCCR1B, CS11);
    BIT_CLR(TCCR1B, CS10);
}

void stepper_heartbeat_disable()
{
    // Stop timer by clearing all clock select bits
    BIT_CLR(TCCR1B, CS12);
    BIT_CLR(TCCR1B, CS11);
    BIT_CLR(TCCR1B, CS10);
}

// helper functions
void stepper_set_disable(Stepper *s, bool disable)
{
    s->disable = disable;
    s->config_dirty = true;
}

void stepper_set_dir(Stepper *s, bool dir)
{
    s->dir = dir;
    s->config_dirty = true;
}

void stepper_set_microstep(Stepper *s, MicrostepMode mode)
{
    s->microstep_mode = mode;
    s->config_dirty = true;
}

void stepper_apply_config(Stepper *s)
{
    if (!s->config_dirty)
        return;

    // Update ENABLE pin (active-low)
    BIT_ASSIGN(*(s->port), s->en_pin, s->disable);

    // Update direction
    BIT_ASSIGN(*(s->port), s->dir_pin, s->dir);

    // Update microstepping mode
    BIT_CLR(*(s->port), s->ms1_pin);
    BIT_CLR(*(s->port), s->ms2_pin);
    BIT_CLR(*(s->port), s->ms3_pin);

    BIT_ASSIGN(*(s->port), s->ms1_pin, (s->microstep_mode & 0b100));
    BIT_ASSIGN(*(s->port), s->ms2_pin, (s->microstep_mode & 0b010));
    BIT_ASSIGN(*(s->port), s->ms3_pin, (s->microstep_mode & 0b001));

    s->config_dirty = false;
}

bool stepper_is_dirty(Stepper* s){
    return s->config_dirty;
}

void stepper_step_n(Stepper* stepper, int steps, bool dir){
    stepper_set_dir(stepper, dir);
    stepper_apply_config(stepper);
    stepper->steps_remaining += steps;
}








// old code
/*
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

*/

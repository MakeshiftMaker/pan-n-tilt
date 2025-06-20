#ifndef STEPPER_H
#define STEPPER_H

#include <avr/io.h>
#include <stdbool.h>

typedef struct {
    volatile uint8_t* ddr;
    volatile uint8_t* port;
    uint8_t step_pin;
    uint8_t dir_pin;
    uint8_t en_pin;
    uint8_t ms1_pin;
    uint8_t ms2_pin;
    uint8_t ms3_pin;
    uint8_t rst_pin;
    uint8_t slp_pin;
} Stepper;

typedef enum
{
    MICROSTEP_FULL = 0b000,
    MICROSTEP_HALF = 0b100,
    MICROSTEP_QUARTER = 0b010,
    MICROSTEP_EIGHTH = 0b110,
    MICROSTEP_SIXTEENTH = 0b111
} MicrostepMode;

void stepper_set_microstep_mode(Stepper* s, MicrostepMode mode);
void stepper_setup(Stepper* s);
void stepper_set_direction(Stepper* s, bool dir);
void stepper_step(Stepper* s);
void stepper_step_n(Stepper* s, int steps);

#endif

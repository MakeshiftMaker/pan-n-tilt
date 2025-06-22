#ifndef STEPPER_H
#define STEPPER_H

#include <avr/io.h>
#include <stdbool.h>
#include <avr/interrupt.h>

typedef enum
{
    MICROSTEP_FULL = 0b000,
    MICROSTEP_HALF = 0b100,
    MICROSTEP_QUARTER = 0b010,
    MICROSTEP_EIGHTH = 0b110,
    MICROSTEP_SIXTEENTH = 0b111
} MicrostepMode;

typedef struct
{
    volatile uint8_t *ddr;
    volatile uint8_t *port;
    const uint8_t step_pin;
    const uint8_t dir_pin;
    const uint8_t en_pin;
    const uint8_t ms1_pin;
    const uint8_t ms2_pin;
    const uint8_t ms3_pin;
    const uint8_t rst_pin;
    const uint8_t slp_pin;

    volatile int steps_remaining;

    // Configuration shadow state
    volatile bool disable; // Desired state of enable pin (false = enable motor)
    volatile bool dir;     // Desired direction
    volatile MicrostepMode microstep_mode;

    // Internal tracking
    volatile bool config_dirty; // Dirty flag to apply config safely
} Stepper;

// Initialization and configuration
void stepper_setup(Stepper *s);
void stepper_apply_config(Stepper *s);

// Configuration setters
void stepper_set_disable(Stepper *s, bool disable);
void stepper_set_dir(Stepper *s, bool dir);
void stepper_set_microstep(Stepper *s, MicrostepMode mode);

bool is_dirty(Stepper* s);

// Heartbeat timer control
void heartbeat_setup();
void heartbeat_enable();
void heartbeat_disable();

#endif

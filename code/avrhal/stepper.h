#ifndef STEPPER_H
#define STEPPER_H

#include <stdbool.h>
#include <stdint.h>     // for uint32_t
#include <avr/io.h>     // for PORTD, DDRD, etc.

// Pin definitions
#define STEP_PIN PD0
#define DIR_PIN  PD1
#define RST_PIN  PD2
#define MS3_PIN  PD3
#define MS2_PIN  PD4
#define MS1_PIN  PD5

// Microstep modes enum
typedef enum
{
    MICROSTEP_FULL       = 0b000,  // MS1=L, MS2=L, MS3=L — Full Step, 2 Phase
    MICROSTEP_HALF       = 0b100,  // MS1=H, MS2=L, MS3=L — Half Step, 1-2 Phase
    MICROSTEP_QUARTER    = 0b010,  // MS1=L, MS2=H, MS3=L — Quarter Step, W1-2 Phase
    MICROSTEP_EIGHTH     = 0b110,  // MS1=H, MS2=H, MS3=L — Eighth Step, 2W1-2 Phase
    MICROSTEP_SIXTEENTH  = 0b111   // MS1=H, MS2=H, MS3=H — Sixteenth Step, 4W1-2 Phase
} MicrostepMode;

// Variables (defined in stepper.c)
extern volatile int stepCount;
extern volatile bool stepInProgress;
extern volatile uint32_t lastStepTime;

// Function declarations
void stepper_setup(void);
void stepper_mode_select(MicrostepMode mode);
void stepper_set_direction(bool dir);
void stepper_tick(void);
void stepper_step_n(int steps);

#endif // STEPPER_H

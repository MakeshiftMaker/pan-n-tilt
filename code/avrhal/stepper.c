#include "time.h"

#include <stdbool.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#define EN_PIN PD0
#define MS1_PIN PD1
#define MS2_PIN PD2
#define MS3_PIN PD3
#define RST_PIN PD4
#define SLP_PIN PD5
#define STEP_PIN PD6
#define DIR_PIN PD7

volatile int stepCount = 0;
volatile bool stepInProgress = false;
volatile uint32_t lastStepTime = 0;

typedef enum
{
    MICROSTEP_FULL = 0b000,     // MS1=L, MS2=L, MS3=L — Full Step, 2 Phase
    MICROSTEP_HALF = 0b100,     // MS1=H, MS2=L, MS3=L — Half Step, 1-2 Phase
    MICROSTEP_QUARTER = 0b010,  // MS1=L, MS2=H, MS3=L — Quarter Step, W1-2 Phase
    MICROSTEP_EIGHTH = 0b110,   // MS1=H, MS2=H, MS3=L — Eighth Step, 2W1-2 Phase
    MICROSTEP_SIXTEENTH = 0b111 // MS1=H, MS2=H, MS3=H — Sixteenth Step, 4W1-2 Phase
} MicrostepMode;

void stepper_setup()
{
    //DDRD = 0;
    //PORTD = 0;
    // Set STEP, DIR, RST, MS1, MS2, MS3 as outputs
    DDRD |= (1 << EN_PIN) | // 1 disable, 0 enable
            (1 << MS1_PIN) |
            (1 << MS2_PIN) |
            (1 << MS3_PIN) |
            (1 << RST_PIN) | // 1 enable step, 0 reset
            (1 << SLP_PIN) | // 1 awake, 0 sleep
            (1 << STEP_PIN) |
            (1 << DIR_PIN);

    // Optional: Initialize outputs to default logic levels
    PORTD &= ~((1 << STEP_PIN) | (1 << DIR_PIN));                 // STEP low, DIR low
    PORTD |= (1 << RST_PIN);                                      // RST high (not in reset)
    PORTD |= (1 << SLP_PIN);                                      // disable sleep mode
    PORTD &= ~(1 << EN_PIN);                                      // clear enable pin to enable
    PORTD &= ~((1 << MS1_PIN) | (1 << MS2_PIN) | (1 << MS3_PIN)); // Default to full step
}

void stepper_mode_select(MicrostepMode mode)
{
    // Clear current MS1–MS3 bits
    PORTD &= ~((1 << MS1_PIN) | (1 << MS2_PIN) | (1 << MS3_PIN));

    if (mode & 0b100)
        PORTD |= (1 << MS1_PIN);
    if (mode & 0b010)
        PORTD |= (1 << MS2_PIN);
    if (mode & 0b001)
        PORTD |= (1 << MS3_PIN);
}

void stepper_set_direction(bool dir)
{
    if (dir)
        PORTD |= (1 << DIR_PIN);
    else
        PORTD &= ~(1 << DIR_PIN);
}

void stepper_tick()
{
    if (!stepInProgress || stepCount <= 0)
        return;

    uint32_t now = timeUSec();

    // 2µs per full pulse (1µs high, 1µs low)
    if ((now - lastStepTime) >= 2)
    {
        // Toggle STEP pin
        PORTD ^= (1 << STEP_PIN);
        lastStepTime = now;

        // Only count falling edge as a complete step
        if ((PORTD & (1 << STEP_PIN)) == 0)
        {
            stepCount--;
            if (stepCount == 0)
            {
                stepInProgress = false;
            }
        }
    }
}

void stepper_step_n(int steps)
{
    if (steps <= 0 || stepInProgress)
        return;

    stepCount = steps;
    stepInProgress = true;
    lastStepTime = timeUSec();

    PORTD &= ~(1 << STEP_PIN); // Start LOW
}

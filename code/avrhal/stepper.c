#include "stepper.h"
#include "../utils/bit.h"
// #include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdlib.h>
#include "usart.h"

#define FCLK 16000000UL

#define GEAR_RATIO 3   // motor steps are 3x gear output
#define STEP_ANGLE 1.8 // degrees per full motor step
#define FULL_ROTATION 360
#define TILT_MIN 0
#define TILT_MAX 90

#define PRESCALER 8


void stepper_setup(Stepper *s)
{
    // Set direction of control pins as output
    // BIT_SET(*(s->ddr), s->step_pin);
    BIT_SET(*(s->ddr), s->dir_pin);
    BIT_SET(*(s->ddr), s->en_pin);
    /*
    BIT_SET(*(s->ddr), s->ms1_pin);
    BIT_SET(*(s->ddr), s->ms2_pin);
    BIT_SET(*(s->ddr), s->ms3_pin);
    BIT_SET(*(s->ddr), s->rst_pin);
    BIT_SET(*(s->ddr), s->slp_pin);
    */

    // Set STEP and DIR low
    /*
    BIT_CLR(*(s->port), s->step_pin);
    BIT_CLR(*(s->port), s->dir_pin);

    // Set RST and SLP high
    BIT_SET(*(s->port), s->rst_pin);
    BIT_SET(*(s->port), s->slp_pin);
    */

    // Set EN (active-low) high = disable driver
    // default disabled
    BIT_SET(*(s->port), s->en_pin);

    // Clear microstep mode bits (full step mode)
    /*
    BIT_CLR(*(s->port), s->ms1_pin);
    BIT_CLR(*(s->port), s->ms2_pin);
    BIT_CLR(*(s->port), s->ms3_pin);
    */

    //_delay_us(5000);
}


void stepper_heartbeat_setup()
{ // set up timer1 to interrupt with frequency f Hz
    // set up the TCCR1A/B registries

    BIT_SET(DDRB, PB1);
    BIT_SET(DDRB, PB2);

    // clear compare output for pb1 (normal operation) but set pb2 to toggle on compare match
    BIT_CLR(TCCR1A, COM1A1);
    BIT_CLR(TCCR1A, COM1A0);
    BIT_CLR(TCCR1A, COM1B1);
    BIT_SET(TCCR1A, COM1B0);
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
    BIT_SET(TIMSK1, OCIE1A);

}

uint16_t get_prescaler_value(TimerPrescaler prescaler) {
    switch (prescaler) {
        case TIMER_CLK_NO_PRESCALE: return 1;
        case TIMER_CLK_8:           return 8;
        case TIMER_CLK_64:          return 64;
        case TIMER_CLK_256:         return 256;
        case TIMER_CLK_1024:        return 1024;
        default:                    return 0; // 0 for NO_CLOCK or external sources
    }
}

void stepper_set_heartbeat(int f, TimerPrescaler prescaler)
{
    OCR1A = (FCLK / (2 * get_prescaler_value(prescaler) * f)) - 1;
}


void stepper_heartbeat_set_clock_prescaler(TimerPrescaler prescaler)
{
    /*
    // Set prescaler to 8: CS12 = 0, CS11 = 1, CS10 = 0
    BIT_CLR(TCCR1B, CS12);
    BIT_SET(TCCR1B, CS11);
    BIT_CLR(TCCR1B, CS10);
    */

    //query each bit in the prescaler enum and set bits accordingly
    BIT_ASSIGN(TCCR1B, CS12, (prescaler & 0b100));
    BIT_ASSIGN(TCCR1B, CS11, (prescaler & 0b010));
    BIT_ASSIGN(TCCR1B, CS10, (prescaler & 0b001));
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
    /*
    BIT_CLR(*(s->port), s->ms1_pin);
    BIT_CLR(*(s->port), s->ms2_pin);
    BIT_CLR(*(s->port), s->ms3_pin);

    BIT_ASSIGN(*(s->port), s->ms1_pin, (s->microstep_mode & 0b100));
    BIT_ASSIGN(*(s->port), s->ms2_pin, (s->microstep_mode & 0b010));
    BIT_ASSIGN(*(s->port), s->ms3_pin, (s->microstep_mode & 0b001));
    */

    s->config_dirty = false;
}

bool stepper_is_dirty(Stepper *s)
{
    return s->config_dirty;
}

void stepper_step_n(Stepper *stepper, int steps, bool dir)
{
    stepper_set_dir(stepper, dir);
    stepper_apply_config(stepper);
    stepper->steps_remaining += steps;
}

uint8_t stepper_microstep_multiplier(MicrostepMode mode)
{
    switch (mode)
    {
    case MICROSTEP_FULL:
        return 16;
    case MICROSTEP_HALF:
        return 8;
    case MICROSTEP_QUARTER:
        return 4;
    case MICROSTEP_EIGHTH:
        return 2;
    case MICROSTEP_SIXTEENTH:
        return 1;
    default:
        return 16; // fallback to full steps
    }
}

void stepper_reset_position(Stepper *stepper)
{
    int32_t steps = stepper->steps_taken;

    if (steps == 0)
        return;

    bool direction = steps > 0 ? 0 : 1; // Reverse direction
    int32_t abs_steps = steps > 0 ? steps : -steps;

    stepper_step_n(stepper, abs_steps, direction);
    // stepper_set_heartbeat(100);
    // stepper->steps_taken = 0;  // Reset to zero here to avoid re-triggering
}

// angles[0] = pan_whole (0..359)
// angles[1] = pan_frac (0..99)
// angles[2] = tilt_whole (0..359)
// angles[3] = tilt_frac (0..99)

void stepper_get_angles(const Stepper *pan, const Stepper *tilt, int angles[4])
{
    int32_t motor_pan_deg_x100 = pan->steps_taken * 180; // steps * 1.8 * 100
    int32_t motor_tilt_deg_x100 = tilt->steps_taken * 180;

    int32_t pan_deg_x100 = motor_pan_deg_x100 / 3;
    pan_deg_x100 %= 36000;
    if (pan_deg_x100 < 0)
        pan_deg_x100 += 36000;

    int32_t tilt_deg_x100 = (motor_tilt_deg_x100 - motor_pan_deg_x100) / 3 + 9000;
    tilt_deg_x100 %= 36000;
    if (tilt_deg_x100 < 0)
        tilt_deg_x100 += 36000;

    angles[0] = pan_deg_x100 / 100;
    angles[1] = pan_deg_x100 % 100;
    angles[2] = tilt_deg_x100 / 100;
    angles[3] = tilt_deg_x100 % 100;
}

// Helper: convert degrees to motor steps (full steps)
static int deg_to_steps(int deg)
{
    return (int)((deg * GEAR_RATIO) / STEP_ANGLE);
}

// Helper: shortest signed difference between two angles 0..359
static int shortest_angle_diff(int current, int target)
{
    int diff = target - current;
    if (diff > 180)
        diff -= 360;
    else if (diff < -180)
        diff += 360;
    return diff;
}

// New go_to function that takes stepper pointers
void stepper_goto_position(Stepper *pan, Stepper *tilt, int target_azimuth, int target_elevation)
{
    if (pan->steps_remaining > 0 || tilt->steps_remaining > 0)
    { // make sure previous goto is finished before starting another
        return;
    }
    // Clamp tilt to limits
    if (target_elevation < TILT_MIN)
        target_elevation = TILT_MIN;
    else if (target_elevation > TILT_MAX)
        target_elevation = TILT_MAX;

    // Get current angles
    int angles[4];
    stepper_get_angles(pan, tilt, angles);

    int current_azimuth = angles[0];   // 0..359 degrees
    int current_elevation = angles[2]; // 0..180 degrees

    // Compute shortest angle deltas
    int delta_azimuth = shortest_angle_diff(current_azimuth, target_azimuth);
    int delta_elevation = target_elevation - current_elevation;

    // Calculate pan steps and direction
    int pan_steps = deg_to_steps(abs(delta_azimuth));
    bool pan_dir = (delta_azimuth < 0);

    // Calculate tilt steps and direction
    int tilt_steps = deg_to_steps(abs(delta_elevation));
    bool tilt_dir = (delta_elevation < 0);

    usartPrint(
        "Goto: Az %d→%d (Δ%d, %d steps %s), El %d→%d (Δ%d, %d steps %s)\r\n",
        current_azimuth, target_azimuth, delta_azimuth, pan_steps, pan_dir ? "CW" : "CCW",
        current_elevation, target_elevation, delta_elevation, tilt_steps, tilt_dir ? "CW" : "CCW");

    // Convert directions to +1/-1 for math
    int pan_dir_sign = pan_dir ? -1 : 1; // assuming dir=1 means clockwise decrement step counter
    int tilt_dir_sign = tilt_dir ? -1 : 1;
    // Calculate net steps for tilt motor
    int tilt_motor_net_steps = pan_steps * pan_dir_sign + tilt_steps * tilt_dir_sign;

    // Queue pan steps (both motors for pan)
    if (pan_steps > 0)
    {
        stepper_step_n(pan, pan_steps, pan_dir);
    }

    // Queue only net steps on tilt motor
    if (tilt_motor_net_steps != 0)
    {
        stepper_step_n(tilt, abs(tilt_motor_net_steps), tilt_motor_net_steps < 0);
    }
}

int calculate_heartbeat_frequency(uint16_t pan_steps, uint16_t tilt_steps, uint8_t min_freq, uint8_t max_freq)
{
    int freq = ((pan_steps / 50 + tilt_steps / 50) * max_freq) + min_freq;
    if (freq < 50)
    {
        freq = 50;
    }
    else if (freq > 150)
    {
        freq = 150;
    }
    return freq;
}

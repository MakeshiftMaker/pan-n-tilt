#include <avr/io.h>
#include <util/delay.h>
#include "avrhal/stepper.h"
#include "utils/bit.h"

Stepper stepper_pan = {
    .ddr = &DDRB,
    .port = &PORTB,
    .step_pin = PB6,
    .dir_pin = PB7,
    .en_pin = PB0,
    .ms1_pin = PB1,
    .ms2_pin = PB2,
    .ms3_pin = PB3,
    .rst_pin = PB4,
    .slp_pin = PB5,

    .steps_taken = 0,
    .steps_remaining = 0,

    .disable = false,
    .dir = false,
    .microstep_mode = MICROSTEP_FULL,

    .config_dirty = false

};

Stepper stepper_tilt = {
    .ddr = &DDRD,
    .port = &PORTD,
    .step_pin = PD6,
    .dir_pin = PD7,
    .en_pin = PD0,
    .ms1_pin = PD1,
    .ms2_pin = PD2,
    .ms3_pin = PD3,
    .rst_pin = PD4,
    .slp_pin = PD5,

    .steps_taken = 0,
    .steps_remaining = 0,

    .disable = false,
    .dir = false,
    .microstep_mode = MICROSTEP_FULL,

    .config_dirty = false};

int main(void)
{
    sei();

    stepper_setup(&stepper_pan);
    stepper_setup(&stepper_tilt);
    stepper_heartbeat_setup(50);

    stepper_set_disable(&stepper_pan, false);
    stepper_set_disable(&stepper_tilt, false);
    stepper_apply_config(&stepper_pan);
    stepper_apply_config(&stepper_tilt);

    stepper_heartbeat_enable();

    stepper_step_n(&stepper_pan, 200*3, 1);
    stepper_step_n(&stepper_tilt, 200*3, 1);

    while (stepper_pan.steps_remaining > 0 || stepper_tilt.steps_remaining > 0)
        ;

    stepper_step_n(&stepper_pan, 200*3, 0);
    stepper_step_n(&stepper_tilt, 200*3, 0);

    while (stepper_pan.steps_remaining > 0 || stepper_tilt.steps_remaining > 0)
        ;

    return 0;
}

ISR(TIMER1_COMPA_vect)
{ // toggle step pins
    if (stepper_pan.steps_remaining > 0)
    {
        BIT_TOGGLE(*(stepper_pan.port), stepper_pan.step_pin); // Toggle PB1 (Pan)
        _delay_us(1);
        BIT_TOGGLE(*(stepper_pan.port), stepper_pan.step_pin); // Toggle PB1 (Pan)
        _delay_us(1);
        stepper_pan.steps_remaining--;
        
        int8_t step_direction = stepper_pan.dir ? 1 : -1;
        stepper_pan.steps_taken += step_direction * stepper_microstep_multiplier(stepper_pan.microstep_mode);
    }

    if (stepper_tilt.steps_remaining > 0)
    {
        BIT_TOGGLE(*(stepper_tilt.port), stepper_tilt.step_pin); // Toggle PD1 (Tilt)
        _delay_us(1);
        BIT_TOGGLE(*(stepper_tilt.port), stepper_tilt.step_pin); // Toggle PD1 (Tilt)
        _delay_us(1);
        stepper_tilt.steps_remaining--;
        
        int8_t step_direction = stepper_tilt.dir ? 1 : -1;
        stepper_tilt.steps_taken += step_direction * stepper_microstep_multiplier(stepper_tilt.microstep_mode);
    }
}

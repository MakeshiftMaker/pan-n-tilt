#include <avr/io.h>
#include <util/delay.h>
#include "stepper.h"

int main(void)
{
    sei();
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

        .steps_remaining = 0,

        .disable = false,
        .dir = false,
        .microstep_mode = MICROSTEP_FULL,

        .config_dirty = false
    };

    stepper_setup(&stepper_pan);
    stepper_setup(&stepper_tilt);
    heartbeat_setup();

    int dir = 0;

    while (1)
    {
        stepper_set_dir(&stepper_pan, dir);
        stepper_set_dir(&stepper_tilt, dir);

        stepper_set_disable(&stepper_pan, true);
        stepper_apply_config(&stepper_pan);

        // dir = !dir;
        heartbeat_enable();
        //_delay_ms(2000);

        // heartbeat_disable();
        //_delay_ms(2000);
    }

    return 0;
}

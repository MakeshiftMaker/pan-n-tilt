#include <avr/io.h>
#include <util/delay.h>
#include "stepper.h"


int main(void)
{
    Stepper stepper1 = {
        .ddr = &DDRB,
        .port = &PORTB,
        .step_pin = PB6,
        .dir_pin = PB7,
        .en_pin = PB0,
        .ms1_pin = PB1,
        .ms2_pin = PB2,
        .ms3_pin = PB3,
        .rst_pin = PB4,
        .slp_pin = PB5
    };

    Stepper stepper2 = {
        .ddr = &DDRD,
        .port = &PORTD,
        .step_pin = PD6,
        .dir_pin = PD7,
        .en_pin = PD0,
        .ms1_pin = PD1,
        .ms2_pin = PD2,
        .ms3_pin = PD3,
        .rst_pin = PD4,
        .slp_pin = PD5
    };

    stepper_setup(&stepper1);
    stepper_setup(&stepper2);

    int dir = 0;

    while (1)
    {
        stepper_set_direction(&stepper1, dir);
        stepper_set_direction(&stepper2, dir);

        stepper_step_n(&stepper1, 200);
        stepper_step_n(&stepper2, 200);

        dir = !dir;
        _delay_ms(2000);
    }

    return 0;
}

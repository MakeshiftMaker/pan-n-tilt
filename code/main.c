#include <avr/io.h>
#include <util/delay.h>
#include "avrhal/stepper.h"
#include "utils/bit.h"
#include "avrhal/joystick.h"
#include "avrhal/usart.h"
#include "avrhal/adc.h"

typedef enum
{
    STOP,
    LEFT,
    RIGHT,
    UP,
    DOWN
} Direction;

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
    usartSetup(USART_B9600, USART_CONFIG_8N1);
    joystickSetup();

    sei();
    int16_t joy[3];

    stepper_setup(&stepper_pan);
    stepper_setup(&stepper_tilt);
    stepper_heartbeat_setup(100);

    stepper_set_disable(&stepper_pan, false);
    stepper_set_disable(&stepper_tilt, false);
    stepper_apply_config(&stepper_pan);
    stepper_apply_config(&stepper_tilt);

    stepper_heartbeat_enable();

    stepper_step_n(&stepper_pan, 200 * 3, 1);
    stepper_step_n(&stepper_tilt, 200 * 3, 1);

    Direction pan_direction = STOP;
    Direction tilt_direction = STOP;


    while (1)
    {
        joystickRead(joy);
        float pan_deg = (stepper_pan.steps_taken / 16.0) * 1.8;
        float tilt_deg = (stepper_tilt.steps_taken / 16.0) * 1.8;

        int pan_whole = (int)pan_deg;
        int pan_frac = (int)((pan_deg - pan_whole) * 100);

        int tilt_whole = (int)tilt_deg;
        int tilt_frac = (int)((tilt_deg - tilt_whole) * 100);

        if (joy[0] < 300)
            pan_direction = LEFT;
        else if (joy[0] > 700)
            pan_direction = RIGHT;
        else
            pan_direction = STOP;

        if (joy[1] < 300)
            tilt_direction = DOWN;
        else if (joy[1] > 700)
            tilt_direction = UP;
        else
            tilt_direction = STOP;

        if (stepper_tilt.steps_remaining == 0)
        {
            switch (Direction)
            {
            case LEFT:
                stepper_step_n(&stepper_pan, 1, 0);
                stepper_step_n(&stepper_tilt, 1, 0);
                break;
            case RIGHT:
                stepper_step_n(&stepper_pan, 1, 1);
                stepper_step_n(&stepper_tilt, 1, 1);
                break;
            case UP:
                stepper_step_n(&stepper_tilt, 1, 0);
                break;
            case DOWN:
                stepper_step_n(&stepper_tilt, 1, 1);
                break;
            case STOP:
                break;
            }
        }

        if (joy[2] == 1) {
            reset_stepper_position(&stepper_pan);
            reset_stepper_position(&stepper_tilt);
        }

        usartPrint(
            "X: %4d, Y: %4d, B: %d | "
            "P: t=%4ld, r=%4d, d=%3d.%02d | "
            "T: t=%4ld, r=%4d, d=%3d.%02d\r\n",
            joy[0], joy[1], joy[2],
            stepper_pan.steps_taken, stepper_pan.steps_remaining, pan_whole, pan_frac,
            stepper_tilt.steps_taken, stepper_tilt.steps_remaining, tilt_whole, tilt_frac);

        // usartPrint("Hello World \r\n");
        //_delay_ms(1000);
    }

    /*stepper_step_n(&stepper_pan, 200*3, 0);
    stepper_step_n(&stepper_tilt, 200*3, 0);

    while (stepper_pan.steps_remaining > 0 || stepper_tilt.steps_remaining > 0)
        ;
    */

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

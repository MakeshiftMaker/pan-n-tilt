#include <avr/io.h>
#include <util/delay.h>
#include "avrhal/stepper.h"
#include "utils/bit.h"
#include "avrhal/joystick.h"
#include "avrhal/usart.h"
#include "avrhal/adc.h"
#include "utils/math.h"
#include "avrhal/commandParser.h"

#define PRESCALER TIMER_CLK_8
#define JOYSTICK_DEADZONE 50
#define MIN_FREQ 100
#define MAX_FREQ 150

Stepper stepper_tilt = {
    .ddr = &DDRD,
    .port = &PORTD,
    //.step_pin = PB6,
    .dir_pin = PD2,
    .en_pin = PD3,
    /*
    .ms1_pin = PB1,
    .ms2_pin = PB2,
    .ms3_pin = PB3,
    .rst_pin = PB4,
    .slp_pin = PB5,
    */
    .steps_taken = 0,
    .steps_remaining = 0,
    .step_pin_state = 0,

    .disable = false,
    .dir = false,
    //.microstep_mode = MICROSTEP_FULL,

    .config_dirty = false

};

Stepper stepper_pan = {
    .ddr = &DDRD,
    .port = &PORTD,
    //.step_pin = PD6,
    .dir_pin = PD4,
    .en_pin = PD6,
    /*
    .ms1_pin = PD1,
    .ms2_pin = PD2,
    .ms3_pin = PD3,
    .rst_pin = PD4,
    .slp_pin = PD5,
    */
    .steps_taken = 0,
    .steps_remaining = 0,
    .step_pin_state = 0,

    .disable = false,
    .dir = false,
    //.microstep_mode = MICROSTEP_FULL,

    .config_dirty = false};

int main(void)
{
    // enable interrupt
    sei();

    // joystick
    joystickSetup();
    int16_t joy[3];
    Direction direction = STOP;

    // steppers
    stepper_setup(&stepper_pan);
    stepper_setup(&stepper_tilt);
    stepper_heartbeat_setup();
    stepper_set_heartbeat(0, PRESCALER);
    stepper_heartbeat_enable(PRESCALER);

    // int16_t max_offset;
    int freq = 0;
    int angles[4];

    // usart setup
    usartSetup(USART_B9600, USART_CONFIG_8N1);

    usartPrint("Setup: OK\n\rp-n-t V2.0.0\n\r");

    while (1)
    {
        stepper_get_angles(&stepper_pan, &stepper_tilt, angles);
        commandParserPoll(&stepper_pan, &stepper_tilt);

        //joystickRead(joy);
        int16_t x_offset = 0; //joy[0] - 510;
        int16_t y_offset = 0; //joy[1] - 495;

        if (x_offset < -JOYSTICK_DEADZONE)
            direction = LEFT;
        else if (x_offset > JOYSTICK_DEADZONE)
            direction = RIGHT;
        else if (y_offset < -JOYSTICK_DEADZONE)
            direction = DOWN;
        else if (y_offset > JOYSTICK_DEADZONE)
            direction = UP;
        else
            direction = STOP;

        // if no joystick input
        if (direction == STOP)
        {
            freq = calculate_heartbeat_frequency(stepper_pan.steps_remaining, stepper_tilt.steps_remaining, MIN_FREQ, MAX_FREQ);
        }
        else
        {
            int16_t max_offset = absInt16(x_offset) > absInt16(y_offset) ? absInt16(x_offset) : absInt16(y_offset);

            // Map offset (0–500ish) to heartbeat frequency (50–150 Hz)
            freq = 50 + (max_offset * 100) / 500;
            if (freq > 150)
                freq = 150;
        }

        stepper_set_heartbeat(freq, PRESCALER);

        if (stepper_tilt.steps_remaining == 0 && stepper_pan.steps_remaining == 0)
        {
            switch (direction)
            {
            case LEFT:
                stepper_step_n(&stepper_pan, 1, 1);
                stepper_step_n(&stepper_tilt, 1, 1);
                break;
            case RIGHT:
                stepper_step_n(&stepper_pan, 1, 0);
                stepper_step_n(&stepper_tilt, 1, 0);
                break;
            case DOWN:
                stepper_step_n(&stepper_tilt, 1, 0);
                break;
            case UP:
                stepper_step_n(&stepper_tilt, 1, 1);
                break;
            case STOP:
                break;
            }
        }

        static int prev_button = 0;
        if (joy[2] == 1 && prev_button == 0)
        {
            stepper_goto_position(&stepper_pan, &stepper_tilt, 0, 90);
        }
        prev_button = joy[2];

        // usartPrint("PD0 (RXD): %d, PD1 (TXD): %d\r\n", pd0_val, pd1_val);
        // usartPrint("test\r\n");

        // usartPrint("Joystick X: %+d, Y: %+d\r\n", joy[0] - 510, joy[1] - 495);
        //usartPrint("Joystick X: %+d, Y: %+d, Freq: %d Hz\r\n", x_offset, y_offset, freq);

        /*
        static int tmp_counter = 0;
        tmp_counter++;
        if (tmp_counter > 500)
        {
            const char *dir_str[] = {"S", "L", "R", "U", "D"};
            usartPrint("D:%s P:%ld/%d T:%ld/%d p:%d t:%d f:%d\r\n",
                       dir_str[direction],
                       stepper_pan.steps_taken, stepper_pan.steps_remaining,
                       stepper_tilt.steps_taken, stepper_tilt.steps_remaining,
                       angles[0], angles[2], freq);
            tmp_counter = 0;
        }
        */
    }

    return 0;
}

ISR(TIMER1_COMPA_vect)
{
    static uint8_t phase = 0;
    phase ^= 1;
    if (phase == 1)
        return; // count only full pulse (2 edges = 1 pulse)

    // --- PAN STEPPER ---
    if (stepper_pan.steps_remaining > 0)
    {
        if (stepper_pan.disable)
        { // enable only if currently disabled
            stepper_set_disable(&stepper_pan, false);
            stepper_apply_config(&stepper_pan);
        }
        stepper_pan.steps_remaining--;
        stepper_pan.steps_taken += (stepper_pan.dir) ? -1 : 1;
    }
    else
    {
        if (!stepper_pan.disable)
        { // disable only if currently enabled
            stepper_set_disable(&stepper_pan, true);
            stepper_apply_config(&stepper_pan);
        }
    }

    // --- TILT STEPPER ---
    if (stepper_tilt.steps_remaining > 0)
    {
        if (stepper_tilt.disable)
        {
            stepper_set_disable(&stepper_tilt, false);
            stepper_apply_config(&stepper_tilt);
        }

        stepper_tilt.steps_remaining--;
        stepper_tilt.steps_taken += (stepper_tilt.dir) ? -1 : 1;
    }
    else
    {
        if (!stepper_tilt.disable)
        {
            stepper_set_disable(&stepper_tilt, true);
            stepper_apply_config(&stepper_tilt);
        }
    }
}

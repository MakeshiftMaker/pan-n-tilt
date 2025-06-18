#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include <stdbool.h>

#define EN_PIN PB0
#define MS1_PIN PB1
#define MS2_PIN PB2
#define MS3_PIN PB3
#define RST_PIN PB4
#define SLP_PIN PB5
#define STEP_PIN PB6
#define DIR_PIN PB7

void stepper_setup()
{
    //DDRD = 0;
    //PORTD = 0;
    // Set STEP, DIR, RST, MS1, MS2, MS3 as outputs
    DDRB |= (1 << EN_PIN) | // 1 disable, 0 enable
            (1 << MS1_PIN) |
            (1 << MS2_PIN) |
            (1 << MS3_PIN) |
            (1 << RST_PIN) | // 1 enable step, 0 reset
            (1 << SLP_PIN) | // 1 awake, 0 sleep
            (1 << STEP_PIN) |
            (1 << DIR_PIN);

    // Optional: Initialize outputs to default logic levels
    PORTB &= ~((1 << STEP_PIN) | (1 << DIR_PIN));                 // STEP low, DIR low
    PORTB |= (1 << RST_PIN);                                      // RST high (not in reset)
    PORTB |= (1 << SLP_PIN);                                      // disable sleep mode
    PORTB &= ~(1 << EN_PIN);                                      // clear enable pin to enable
    PORTB &= ~((1 << MS1_PIN) | (1 << MS2_PIN) | (1 << MS3_PIN)); // Default to full step
}

void stepper_set_direction(bool dir)
{
    if (dir)
        PORTB |= (1 << DIR_PIN);
    else
        PORTB &= ~(1 << DIR_PIN);
}

void stepper_step(){
    PORTB &= ~(1 << STEP_PIN); // Start LOW
    _delay_ms(1);
    PORTB |= (1 << STEP_PIN); // Start Pull_High
    _delay_us(2);
    PORTB &= ~(1 << STEP_PIN); // Pull LOW
    _delay_us(2);
}

int main(int argc, char const *argv[])
{
    stepper_setup();
    stepper_set_direction(0);
    _delay_ms(200);

    for(int i = 0 ; i < 10 ; i++){
        stepper_step();
    }
    return 0;
}

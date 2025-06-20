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

typedef enum
{
    MICROSTEP_FULL = 0b000,     // MS1=L, MS2=L, MS3=L — Full Step, 2 Phase
    MICROSTEP_HALF = 0b100,     // MS1=H, MS2=L, MS3=L — Half Step, 1-2 Phase
    MICROSTEP_QUARTER = 0b010,  // MS1=L, MS2=H, MS3=L — Quarter Step, W1-2 Phase
    MICROSTEP_EIGHTH = 0b110,   // MS1=H, MS2=H, MS3=L — Eighth Step, 2W1-2 Phase
    MICROSTEP_SIXTEENTH = 0b111 // MS1=H, MS2=H, MS3=H — Sixteenth Step, 4W1-2 Phase
} MicrostepMode;

void stepper_mode_select(MicrostepMode microstep){

}

void stepper_setup()
{
    // DDRD = 0;
    // PORTD = 0;
    //  Set STEP, DIR, RST, MS1, MS2, MS3 as outputs
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

void stepper_step()
{
    PORTB |= (1 << STEP_PIN); // Start Pull_High
    _delay_us(3000);
    PORTB &= ~(1 << STEP_PIN); // Pull LOW
    _delay_us(3000);
}

int main(int argc, char const *argv[])
{
    int dir = 0;

    stepper_setup();
    _delay_us(5000);
    
    _delay_us(5000);

    
    while (1)
    {
        stepper_set_direction(dir);
        _delay_us(5000);
        for (int i = 0; i < 200; i++)
        {
            stepper_step();
        }
        dir = !dir;
        _delay_ms(2000);
    }
    return 0;
}

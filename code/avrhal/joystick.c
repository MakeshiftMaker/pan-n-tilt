#include "adc.h"
#include "usart.h"
#include "utils/bit.h"
#include <avr/io.h>
#include "joystick.h"


void joystickSetup(){
    adcSetup();
    adcSetupFreeRunning();
}


//eine funktion die die ausgelesen Werte als Array zurückgibt
void joystickRead(int16_t data[3])
{
    data[0] = adcLastRead(0);                     // X-Achse
    data[1] = adcLastRead(1);                     // Y-Achse
    data[2] = !(PINC & (1 << PC2));               // Button gedrückt = 1
}   

Direction joystick_get_direction(int16_t x, int16_t y, int16_t jostick_deadzone){

        if (x_offset < -joystick_deadzone)
            return LEFT;
        else if (x_offset > joystick_deadzone)
            return RIGHT;
        else if (y_offset < -joystick_deadzone)
            return DOWN;
        else if (y_offset > joystick_deadzone)
            return UP;
        else
            return STOP;
}
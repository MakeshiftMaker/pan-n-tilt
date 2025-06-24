#include "adc.h"
#include "usart.h"
#include "utils/bit.h"
#include <avr/io.h>


void joystickSetup(){
    adcSetup();
    adcSetupFreeRunning();
}


//eine funktion die die ausgelesen Werte als Array zurückgibt
void joystickRead(int16_t data[3])
{
    data[0] = adcLastRead(0);                     // X-Achse
    data[1] = adcLastRead(1);                     // Y-Achse
    data[2] = !(PINA & (1 << PA2));               // Button gedrückt = 1
}
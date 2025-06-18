#include "avrhal/adc.h"
#include "avrhal/time.h"
#include "avrhal/usart.h"
#include "avrhal/stepper.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{

    usartSetup(USART_B9600, USART_CONFIG_8N1);
    adcSetup();
    stepper_setup();

    sei();

    usartWriteString("Hello to Lab 4 :-) \n\r");

    int dir = 0;

    volatile int16_t x = 0, y = 0;
    while (1)
    {
        adcSetChannel(0);
        x = adcRead();
        adcSetChannel(1);
        y = adcRead();

        
        
        dir = !dir;
        stepper_set_direction(dir);
        stepper_step_n(20);
        
        usartPrint("X:%d Y:%d DIR:%d\r\n", x, y, dir, );

        _delay_ms(1000);


    }
    return 0;
}

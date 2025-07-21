#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    STOP,
    LEFT,
    RIGHT,
    UP,
    DOWN
} Direction;

void joystickSetup(void);
void joystickRead(int16_t data[3]);
Direction joystick_get_direction(int16_t x, int16_t y, int16_t jostick_deadzone);

#endif

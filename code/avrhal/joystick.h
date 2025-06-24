#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <stdint.h>
#include <stdbool.h>

void joystickSetup(void);
void joystickRead(int16_t data[3]);

#endif

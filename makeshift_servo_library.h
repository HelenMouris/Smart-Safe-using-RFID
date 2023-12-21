#ifndef MAKESHIFT_SERVO_LIBRARY_H
#define MAKESHIFT_SERVO_LIBRARY_H

#include "pico/stdlib.h"
#include "hardware/pwm.h"

void setPosition(uint servoPin, float millis);
void setServo(uint servoPin, float startPosition);

#endif // MAKESHIFT_SERVO_LIBRARY_H
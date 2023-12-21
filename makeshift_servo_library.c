#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/pwm.h"

#define MIN_PULSE_WIDTH 544
#define MAX_PULSE_WIDTH 2400

#define CLOCK_DIVIDER 64.0f
#define WRAP_COUNT 39062.0f

void setPosition(uint servoPin, float angle)
{
    if (angle < 0.f)
    {
        angle = 0.f;
    }
    else if (angle > 180.f)
    {
        angle = 180.f;
    }
    // converting angle to millis (0-180 to 544-2400)
    float millis = (angle / 180.f) * (MAX_PULSE_WIDTH - MIN_PULSE_WIDTH) + MIN_PULSE_WIDTH;

    pwm_set_gpio_level(servoPin, (millis / 20000.f) * WRAP_COUNT);
}

void setServo(uint servoPin, float startPosition)
{
    // initialize the pin as output
    gpio_init(servoPin);
    gpio_set_dir(servoPin, GPIO_OUT);

    stdio_init_all();

    // Set up PWM
    gpio_set_function(servoPin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(servoPin);

    pwm_config config = pwm_get_default_config();
    // Set divider, reduces counter clock to sysclock/this value
    pwm_config_set_clkdiv(&config, CLOCK_DIVIDER);
    // Set wrap, number of counter cycles before reset
    pwm_config_set_wrap(&config, WRAP_COUNT);

    pwm_init(slice_num, &config, true);

    // Set initial position of servo
    setPosition(servoPin, startPosition);
}
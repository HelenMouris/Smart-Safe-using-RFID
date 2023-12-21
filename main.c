#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/uart.h"
#include "hardware/pwm.h"
#include "makeshift_servo_library.h"
#include "mfrc522.h"
#include "ultrasonic.h"

const uint SERVO_PIN = 19;
bool direction = true;
float servoAngle = 0.f;

uint trigPin = 6;
uint echoPin = 7;

float counter = 10;
float clockClose = 5;
bool isYellow = true;

void main() {
    stdio_init_all();
    setupUltrasonicPins(trigPin, echoPin);
    setServo(SERVO_PIN , servoAngle );

    const uint red_pin = 18; 
    const uint green_pin = 17; 
    const uint blue_pin = 16; 

    const uint red_pin2 = 8; 
    const uint green_pin2 = 9; 
    const uint blue_pin2 = 10; 

    const uint buzzer_pin = 20; 
    gpio_init(buzzer_pin);
    gpio_set_dir(buzzer_pin, GPIO_OUT);
    gpio_put(buzzer_pin, true);

    gpio_init(red_pin);
    gpio_init(green_pin);
    gpio_init(blue_pin);

    gpio_init(red_pin2);
    gpio_init(green_pin2);
    gpio_init(blue_pin2);

    gpio_set_dir(red_pin, GPIO_OUT);
    gpio_set_dir(green_pin, GPIO_OUT);
    gpio_set_dir(blue_pin, GPIO_OUT);

    gpio_set_dir(red_pin2, GPIO_OUT);
    gpio_set_dir(green_pin2, GPIO_OUT);
    gpio_set_dir(blue_pin2, GPIO_OUT);

    // Declare card UID's
    uint8_t tag1[] = {0x82, 0xfb, 0xbe, 0x4e};

    MFRC522Ptr_t mfrc = MFRC522_Init();
    PCD_Init(mfrc, spi0);

    gpio_put(red_pin2, true);
    gpio_put(green_pin2, false);
    gpio_put(blue_pin2, true);

    //sleep_ms(10000);

    while(1) {
        gpio_put(red_pin, false);
        gpio_put(green_pin, false);
        gpio_put(blue_pin, true);

        //Wait for new card
        printf("Waiting for card\n\r");
        while(!PICC_IsNewCardPresent(mfrc)) {
            int fofa = getCm(trigPin, echoPin);
            printf("\n %d cm", fofa); 
            // to make a delay so servo doesn't close when if first opens.
            if (clockClose != 0) {
                clockClose--;
            }
            // if distance between servo and ultrasonic is less than 5 (door is closed), close the servo
            if (fofa != 0 && fofa < 5) {
                if (clockClose == 0) {
                    sleep_ms(2000);
                    if (servoAngle > 0) {
                        servoAngle += -110.f;
                        setPosition(SERVO_PIN , servoAngle);
                        printf("Current angle: %f\n", servoAngle);
                    }
                }
            }
            sleep_ms(500);
            counter--;
            
            // concurrent task of LED that switches its color
            if (counter == 0) {
                counter = 10;
                if (isYellow) {
                    isYellow = false;
                    gpio_put(red_pin2, false);
                    gpio_put(green_pin2, true);
                    gpio_put(blue_pin2, true);
                } else {
                    isYellow = true;
                    gpio_put(red_pin2, true);
                    gpio_put(green_pin2, false);
                    gpio_put(blue_pin2, true);
                }
            }
        };
        //Select the card
        printf("Selecting card\n\r");
        PICC_ReadCardSerial(mfrc);

        //Show UID on serial monitor
        printf("PICC dump: \n\r");
        PICC_DumpToSerial(mfrc, &(mfrc->uid));

        //Authorization with uid
        printf("Uid is: ");
        for(int i = 0; i < 4; i++) {
            printf("%x ", mfrc->uid.uidByte[i]);
        } printf("\n\r");

        if(memcmp(mfrc->uid.uidByte, tag1, 4) == 0) {
            gpio_put(red_pin, false);
            gpio_put(green_pin, true);
            gpio_put(blue_pin, false);
            printf("Authentication Success\n\r");
            gpio_put(buzzer_pin, false);
            sleep_ms(500);
            gpio_put(buzzer_pin, true);
            clockClose = 5;
            if (servoAngle < 110) {
                servoAngle +=  110.f;
                setPosition(SERVO_PIN , servoAngle);
                printf("Current angle: %f\n", servoAngle);
            }
        } else {
            gpio_put(red_pin, true);
            gpio_put(green_pin, false);
            gpio_put(blue_pin, false);
            printf("Authentication Failed\n\r");
            gpio_put(buzzer_pin, false);
            sleep_ms(250);
            gpio_put(buzzer_pin, true);
            sleep_ms(250);
            gpio_put(buzzer_pin, false);
            sleep_ms(250);
            gpio_put(buzzer_pin, true);
        }
        sleep_ms(500);
        // concurrent task of LED that switches its color
        counter--;
        if (counter == 0) {
            counter = 10;
            if (isYellow) {
                isYellow = false;
                gpio_put(red_pin2, false);
                gpio_put(green_pin2, true);
                gpio_put(blue_pin2, true);
            } else {
                isYellow = true;
                gpio_put(red_pin2, true);
                gpio_put(green_pin2, false);
                gpio_put(blue_pin2, true);
            }
        }
    }
}
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <wiringPiI2C.h>
#include <math.h>
#include <unistd.h>

// Define hardware pins
#define SCL_PIN 15
#define SDA_PIN 14
#define JOYSTICK_BUTTON_PIN 22
#define LED_R_PIN 12
#define LED_G_PIN 13
#define LED_B_PIN 11
#define BUZZER_PIN 21
#define NP_PIN 7
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6
#define VRX_PIN 26
#define VRY_PIN 27
#define MIC_PIN 28

#define NUM_LEDS 25

// Define colors
int RED[3] = {50, 0, 0};
int GREEN[3] = {0, 50, 0};
int BLUE[3] = {0, 0, 50};
int YELLOW[3] = {30, 30, 0};
int MAGENTA[3] = {30, 0, 30};
int CYAN[3] = {0, 30, 30};
int WHITE[3] = {25, 25, 25};
int BLACK[3] = {0, 0, 0};

// Initialize hardware components
int i2c_fd;
int led_r, led_g, led_b, buzzer;
int adc_vrx, adc_vry, adc_mic;
int np[NUM_LEDS][3]; // NeoPixel array

void setup() {
    wiringPiSetup();
    i2c_fd = wiringPiI2CSetup(0x3C); // OLED I2C address
    pinMode(JOYSTICK_BUTTON_PIN, INPUT);
    pullUpDnControl(JOYSTICK_BUTTON_PIN, PUD_UP);

    // Initialize RGB LEDs
    led_r = LED_R_PIN;
    led_g = LED_G_PIN;
    led_b = LED_B_PIN;
    pinMode(led_r, PWM_OUTPUT);
    pinMode(led_g, PWM_OUTPUT);
    pinMode(led_b, PWM_OUTPUT);
    softPwmCreate(led_r, 0, 100);
    softPwmCreate(led_g, 0, 100);
    softPwmCreate(led_b, 0, 100);

    // Initialize buzzer
    buzzer = BUZZER_PIN;
    pinMode(buzzer, PWM_OUTPUT);

    // Initialize ADC pins
    adc_vrx = VRX_PIN;
    adc_vry = VRY_PIN;
    adc_mic = MIC_PIN;

    // Initialize buttons
    pinMode(BUTTON_A_PIN, INPUT);
    pinMode(BUTTON_B_PIN, INPUT);
    pullUpDnControl(BUTTON_A_PIN, PUD_UP);
    pullUpDnControl(BUTTON_B_PIN, PUD_UP);
}

void update_oled(char* lines[], int count) {
    // Clear the OLED screen
    for (int i = 0; i < count; i++) {
        wiringPiI2CWriteReg8(i2c_fd, 0x00, lines[i]);
    }
}

void gradual_light_sound(int duration) {
    int step_duration = duration / 100;
    for (int i = 0; i <= 100; i++) {
        int duty_cycle = (i * 100) / 200;
        softPwmWrite(led_r, duty_cycle);
        softPwmWrite(led_g, duty_cycle);
        softPwmWrite(led_b, duty_cycle);
        pwmWrite(buzzer, 50 + i * 2);
        delay(step_duration);
    }
    softPwmWrite(led_r, 0);
    softPwmWrite(led_g, 0);
    softPwmWrite(led_b, 0);
    pwmWrite(buzzer, 0);
}

void beep(int freq, float duration) {
    pwmWrite(buzzer, freq);
    softPwmWrite(buzzer, 50);
    delay(duration * 1000);
    pwmWrite(buzzer, 0);
}

void clear_all() {
    for (int i = 0; i < NUM_LEDS; i++) {
        np[i][0] = 0;
        np[i][1] = 0;
        np[i][2] = 0;
    }
}

void heartbeat_effect(int* sequence, int length) {
    int initial_delay = 300;
    int decrement = 50;
    clear_all();
    for (int i = 0; i < length; i++) {
        softPwmWrite(sequence[i], 255);
        delay(initial_delay - i * decrement);
    }
    beep(1000, 0.2);
    delay(300);
    clear_all();
}

void heart() {
    clear_all();
    int heart_leds[] = {2, 6, 14, 15, 23, 17, 21, 19, 10, 8};
    for (int i = 0; i < 10; i++) {
        np[heart_leds[i]][0] = 255;
        np[heart_leds[i]][1] = 0;
        np[heart_leds[i]][2] = 0;
    }
}

int map_value(int value, int in_min, int in_max, int out_min, int out_max) {
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int main(void) {
    setup();

    char* messages[] = {
        "           ",
        "           ",
        "  OLA  HUMANO!",
        "           ",
        "           ",
        "           ",
        "           ",
        "           "
    };

    clear_all();
    gradual_light_sound(2);

    // LED heartbeat sequence
    int sequence[] = {14, 6, 12, 17, 21, 10};
    for (int i = 0; i < 3; i++) {
        heartbeat_effect(sequence, 6);
        heart();
        delay(200);
    }

    update_oled(messages, 8);
    delay(1500);

    clear_all();
    delay(1500);

    while (1) {
        // Implement the rest of the functionality here
    }

    return 0;
}

#pragma once
#include <Arduino.h>
#include <Servo.h>

#define PIR_SENSOR D7               // human sensor
#define INFRARED_OBSTICLE_SENSOR D5 // sword sensor, inverted logic
#define STEPER_MOTOR D6             // steper control

#define PIN_IN D2
#define PIN_OUT D1

bool all_pass{false}; // defines user unlock

unsigned short int probability_to_pass{10}; // probability to pass
char nacin_rada{3};                         // 3 nacina rada sustava

Servo motor;

constexpr unsigned short int motor_unlock{MIN_PULSE_WIDTH - 1};
constexpr unsigned short int motor_lock{MAX_PULSE_WIDTH + 1};
constexpr unsigned short int motor_stop{1500};

// function declaration
bool is_sword_present();
bool is_human_detected();
bool random_chance_to_release();

bool lock_flag{false};
bool unlock_flag{false};

bool presistane_flag{false};
bool is_king{false};
int random_number_holder{-1};

void lock_sword();
void unlock_sword();

#pragma once
#include <Arduino.h>
#include <Servo.h>

#define PIR_SENSOR D7               // human sensor
#define INFRARED_OBSTICLE_SENSOR D5 // sword sensor, inverted logic
#define STEPER_MOTOR D6             // steper control

#define PIN_IN D1
#define PIN_OUT D2

bool all_pass{false}; // defines user unlock

unsigned int probability_to_pass{20}; // probability to pass
int nacin_rada{3};                    // 3 nacina rada sustava

Servo motor;
int motor_lock{180};
int motor_unlock{0};
int motor_stop{90};

bool is_sword_present();
bool is_human_detected();
bool random_chance_to_release();
void lock_sword();
void unlock_sword();
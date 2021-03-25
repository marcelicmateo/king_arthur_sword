#pragma once
#include <Arduino.h>
#include <Servo.h>

#define PIR_SENSOR D1               // human sensor
#define INFRARED_OBSTICLE_SENSOR D2 // sword sensor
#define STEPER_MOTOR D5             // steper control

#define PIN_IN D7  // PULLUP, reverse logic
#define PIN_OUT D6 // PULLUP, reverse logic

bool all_pass{false}; // defines user unlock

unsigned int probability_to_pass{20}; // probability to pass

Servo motor;
int motor_lock{180};
int motor_unlock{180};
int motor_stop{0};

bool is_sword_present();
bool is_human_detected();
bool random_chance_to_release();
void lock_sword();
void unlock_sword();
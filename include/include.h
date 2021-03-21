#pragma once
#include <Arduino.h>
#include <Servo.h>

#define PIR_SENSOR D1 // human sensor
#define INFRARED_OBSTICLE_SENSOR D2 // sword sensor
#define STEPER_MOTOR D6 // steper control



bool all_pass{false}; //defines user unlock

unsigned int probability_to_pass{20}; // probability to pass


Servo motor;
int motor_position{0};
int locked_position{180};
int unlocked_position{0};


bool is_sword_present();
bool is_human_detected();
bool random_chance_to_release();
void lock_sword();
void unlock_sword();
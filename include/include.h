#pragma once
#include <Arduino.h>
#include <Servo.h>

void init_gpio();

#define PIR_SENSOR D7 // human sensor
#define INFRARED_OBSTICLE_SENSOR D5 // sword sensor, inverted logic
#define STEPER_MOTOR D6 // steper control

#define PIN_IN D2
#define PIN_OUT D1

bool all_pass{false}; // defines user unlock

unsigned short int probability_to_pass{10}; // probability to pass
char state{3};                              // 3 nacina rada sustava

Servo motor;

constexpr unsigned short int motor_unlock{MIN_PULSE_WIDTH - 1};
constexpr unsigned short int motor_lock{MAX_PULSE_WIDTH + 1};
constexpr unsigned short int motor_stop{1500};
constexpr unsigned short int motor_move_time{1 * 1000};

// function declaration
bool is_sword_present();
bool is_human_detected();
bool random_chance_to_release();

bool lock_flag{false};
bool unlock_flag{false};

bool presistane_flag{false};
bool is_king{false};
int random_number_holder{-1};

bool lock_sword();
bool unlock_sword();
/*
 * Error handling variables and functions
 *
 */
bool flag_error_lock{false};
bool flag_error_stuck{false};

void error_motor_stuck();
void error_motor_cant_lock();

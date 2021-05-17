#pragma once

#include "LittleFS.h"
#include <Arduino.h>
#include <Servo.h>

void init_gpio_pins();

#define PIR_SENSOR D7               // human sensor
#define INFRARED_OBSTICLE_SENSOR D6 // sword sensor, inverted logic
#define STEPER_MOTOR D1             // steper control

#define PIN_IN D3
#define PIN_OUT D4

bool all_pass{false}; // defines user unlock

uint16 probability_for_king{10};
uint8_t state{0b10000000}; // 3 nacina rada sustava
bool f_generate_new_bingo = 0;
String system_buffer = {};
bool lock{0};
Servo motor;

constexpr unsigned short int motor_unlock{MIN_PULSE_WIDTH - 1};
constexpr unsigned short int motor_lock{MAX_PULSE_WIDTH + 1};
constexpr unsigned short int motor_stop{1500};
constexpr unsigned short int motor_move_time{1 * 1000};

// function declaration
bool is_sword_present();
bool is_human_detected();

bool lock_flag{false};
bool unlock_flag{false};

bool presistane_flag{false};

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

bool is_sword_present() {
  // negative logic sensor
  // mask 1st bit and flip
  // explicit cast to bool for controll
  return (bool)~(digitalRead(INFRARED_OBSTICLE_SENSOR) & 0b01);
}

uint8 pir_states() {
  // 01 -- risng edge, new human
  // 10 -- falling edge, human is off
  // 11 -- constatnt movement
  // 00 -- no movement
  // read and return PIR sensor value
  // explicit cast to bool for controll
  static uint8 pir_state{0};

  pir_state = pir_state << 1;
  pir_state = pir_state | (digitalRead(PIR_SENSOR) & 0b01);

  return pir_state & 0b11; // only lover 2 byte are interesting
}

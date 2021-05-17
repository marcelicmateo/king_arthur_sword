#pragma once

#include "LittleFS.h"
#include <Arduino.h>
#include <Servo.h>

#define PIR_SENSOR D7               // human sensor
#define INFRARED_OBSTICLE_SENSOR D6 // sword sensor, inverted logic
#define STEPER_MOTOR D5             // steper control

#define PIN_IN D1
#define PIN_OUT D2

void init_gpio_pins();

bool all_pass{false}; // defines user unlock

uint16 probability_for_king{10};
uint8_t state{0b10000000}; // 3 nacina rada sustava
bool f_generate_new_bingo = 0;
String system_buffer = {};
bool lock{0};
Servo motor;

constexpr unsigned short int motor_unlock{70};
constexpr unsigned short int motor_lock{110};
constexpr unsigned short int motor_stop{90};
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
  return (bool)((digitalRead(INFRARED_OBSTICLE_SENSOR) & 0b01) ^ 0b01);
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

void write_to_file(String s, String file_name) {
  Serial.println("Writing to file.\n" + s);

  File f = LittleFS.open(file_name, "w");
  for (uint i = 0; i < s.length(); i++) {
    f.write(s[i]);
  }
  f.close();
}

String read_from_file(String file_name) {
  String buff{""};
  if (LittleFS.exists(file_name)) {
    File f = LittleFS.open(file_name, "r");
    buff = f.readString();
    Serial.println("Read: " + buff);
    f.close();
  } else {
    Serial.println("Error reading.\nFile >" + file_name + "< does not exist");
  }
  return buff;
}

constexpr char STATE_txt[]{"State.txt"};

void save_state(uint8_t s) {
  state = s;
  write_to_file((String)s, STATE_txt);
  return;
}

void load_state() {
  String data = read_from_file(STATE_txt);
  if (data == "") {
    Serial.println("New state file");
    save_state(state);
    return;
  }
  state = (uint8_t)(data.toInt());
  Serial.println("Loaded state: " + data);
}

constexpr char HOLD_txt[]{"Hold.txt"};
uint8 hold = 0;

void save_hold() {
  write_to_file((String)hold, HOLD_txt);
  return;
}
void load_hold() {
  String data = read_from_file(HOLD_txt);
  if (data == "") {
    Serial.println("New HOld file");
    save_hold();
    return;
  }
  hold = (uint8_t)(data.toInt());
  Serial.println("Loaded hold: " + data);
}

constexpr char UNL_FLAGS_txt[]{"UFlags.txt"};
constexpr char L_FLAGS_txt[]{"LFlags.txt"};

void save_flags() {
  write_to_file((String)unlock_flag, UNL_FLAGS_txt);
  write_to_file((String)lock_flag, L_FLAGS_txt);
  return;
}

void load_flags() {
  String data1 = read_from_file(UNL_FLAGS_txt);
  String data2 = read_from_file(L_FLAGS_txt);
  if (data1 == "" || data2 == "") {
    save_flags();
    return;
  }
  unlock_flag = (bool)(data1.toInt());
  lock_flag = (bool)(data2.toInt());
  Serial.println("Loaded flags: " + data1 + ", " + data2);
}

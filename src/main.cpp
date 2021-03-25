#include "include.h"
#include "server_setup.h"
#include "wifi_setup.h"

#define DEBUG false
#define Serial                                                                 \
  if (DEBUG)                                                                   \
  Serial

void setup() {
  Serial.begin(115200);
  // defining pin modes
  pinMode(PIR_SENSOR, INPUT);
  pinMode(INFRARED_OBSTICLE_SENSOR, INPUT);
  pinMode(PIN_IN, INPUT_PULLUP);
  pinMode(PIN_OUT, INPUT_PULLUP);
  motor.attach(STEPER_MOTOR);
  unlock_sword();

  wifi_setup();
  server_setup();
  Serial.println("Begin loop");
}

void loop() {

  if (is_sword_present()) {
    Serial.println("Sword detected");
    if (!all_pass) {
      if (is_human_detected() && random_chance_to_release()) {
        unlock_sword();
        delay(5000); // cekaj da se mac izvuce
      } else {
        lock_sword();
      }
    } else {
      unlock_sword();
    }
  }
}

bool is_sword_present() {
  static bool b{true};
  b = (bool)digitalRead(INFRARED_OBSTICLE_SENSOR) ^
      1; // negative logic senor so XOR for positive logic
  return b;
}

bool is_human_detected() {
  static bool b{false};
  b = (bool)digitalRead(PIR_SENSOR);
  return b;
}

bool random_chance_to_release() {
  constexpr int MAX_RANDOM_NUMBER{99}; // constant
  static bool b{false};
  static unsigned int chance{0};
  chance = rand() % MAX_RANDOM_NUMBER;
  Serial.println("Chance to be a king: " + (String)chance);
  if (chance < probability_to_pass)
    b = true;
  else
    b = false;
  return b;
}

void lock_sword() {
  Serial.println("Locking sword");
  while (digitalRead(PIN_IN)) {
    motor.write(motor_unlock);
  }
  motor.write(motor_stop);
  Serial.println("Sword locked");

  return;
}
void unlock_sword() {

  Serial.println("Unlocking sword");
  while (digitalRead(PIN_OUT)) {
    motor.write(motor_unlock);
  }
  motor.write(motor_stop);
  Serial.println("Sword unlocked");

  return;
}
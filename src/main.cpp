#include "include.h"
#include "server_setup.h"
#include "wifi_setup.h"

#define DEBUG true
#define Serial                                                                 \
  if (DEBUG)                                                                   \
  Serial

void setup() {
  Serial.begin(9600);
  // defining pin modes
  pinMode(PIR_SENSOR, INPUT);
  pinMode(INFRARED_OBSTICLE_SENSOR, INPUT);
  motor.attach(STEPER_MOTOR);
  motor.write(motor_position);

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
  static bool b{false};
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
  constexpr int MAX_RANDOM_NUMBER{100};
  static bool b{false};
  static unsigned int chance{0};
  chance = rand() % MAX_RANDOM_NUMBER;
  if (chance < probability_to_pass)
    b = true;
  else
    b = false;
  return b;
}

void lock_sword() {
  if (motor_position == locked_position)
    return;

  Serial.println("Locking sword");
  motor_position = locked_position;
  motor.write(motor_position);
  Serial.println("Sword Locked");
  return;
}
void unlock_sword() {
  if (motor_position == unlocked_position)
    return;

  Serial.println("Unlocking sword");
  motor_position = unlocked_position;
  motor.write(motor_position);
  Serial.println("Sword unlocked");
  return;
}
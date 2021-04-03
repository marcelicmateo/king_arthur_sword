#include "include.h"
#include "server_setup.h"
#include "wifi_setup.h"

#define DEBUG false
#define Serial                                                                 \
  if (DEBUG)                                                                   \
  Serial

void setup() {
  Serial.begin(9600);
  //   defining pin modes

  init_gpio();
  wifi_setup();
  server_setup();

  if (!lock_flag && !unlock_flag) {
    unlock_sword();
  }
  Serial.println("Begin loop");
}

void sword_prema_unlocked() {
  Serial.println("State1");
  unlock_sword();
  return;
}

void sword_prema_locked() {
  Serial.println("State 2");
  lock_sword();
  return;
}

void sword_random_chance_release() {
  Serial.println("State 3");
  static bool detection_flag{presistane_flag};
  detection_flag = is_human_detected();

  if (presistane_flag == false && detection_flag == true) {
    presistane_flag = true;
    if (is_sword_present()) {
      lock_sword();
      if (random_chance_to_release()) {
        is_king = true;
        unlock_sword();
      } else {
        is_king = false;
      }
    }
  }
  if (presistane_flag == true && detection_flag == false) {
    presistane_flag = false; // reset presistance flag
  }
  return;
}

unsigned long current_milis = millis();
unsigned long previous_milis = 0;

void loop() {

  AsyncElegantOTA.loop(); // service OTA updates

  switch (state) { // state machine
  case 256:
    // critical error, system halt
    Serial.println("Critical error, system halt");
    break;
  case 255:
    // lock error
    Serial.println("Sword cant be locked");
    break;
  case 1:
    sword_prema_unlocked();
    break;
  case 2:
    sword_prema_locked();
    break;
  case 3:
    // sword released based on random chance
    // run program every second
    current_milis = millis();
    if (current_milis - previous_milis <= 1000) {
      yield();
    } else {
      previous_milis = current_milis;
      sword_random_chance_release();
    }
    break;
  default:
    // 0
    // should never come to this value
    ESP.restart();
    break;
  }

  yield(); // let ESP do stuff
}

bool is_sword_present() {
  // negative logic sensor
  // mask 1st bit and flip
  // explicit cast to bool for controll
  return (bool)~(digitalRead(INFRARED_OBSTICLE_SENSOR) & 0b01);
}

bool is_human_detected() {
  // read and return PIR sensor value
  // explicit cast to bool for controll
  return (bool)digitalRead(PIR_SENSOR) & 0b01;
}

bool random_chance_to_release() {
  constexpr unsigned short int MAX_RANDOM_NUMBER{99}; // constant
  random_number_holder = rand() % MAX_RANDOM_NUMBER;
  Serial.println("Chance to be a king: " + (String)random_number_holder);
  // check randomised number against defined probability and return acordingly
  if (random_number_holder < probability_to_pass)
    return true;
  else
    return false;
}

bool lock_sword() {
  bool b{0};
  b = digitalRead(PIN_IN);
  if (lock_flag == true) {
    Serial.println("Sword already locked");
    motor.writeMicroseconds(motor_stop);
    return true;
  }
  Serial.println("Locking sword");
  unsigned long pMillis = millis();
  unsigned long cMillis = pMillis;

  motor.writeMicroseconds(motor_lock);
  while (b == LOW) {
    // periodicaly read unlock stop pin
    b = digitalRead(PIN_IN);
    if (cMillis - pMillis >= motor_move_time) {
      // if 1 second has passed and motor isn't unlocked. Motor there is a
      // block in a way of pin
      // try to unlock
      unlock_flag = false; // we moved pin, somewere in the middle
      if (unlock_sword() == true) {
        // stop motor, and raise critical error
        error_motor_cant_lock();
        return false;
      } else {
        return false;
      }
    }
    cMillis = millis();
    yield(); // wdt reset prevention
  }

  // debounce motor back to 0 position
  motor.writeMicroseconds(motor_stop + 300);
  yield();
  delay(100);
  motor.writeMicroseconds(motor_stop + 100);
  yield();
  delay(100);
  motor.writeMicroseconds(motor_stop);

  unlock_flag = false;
  lock_flag = true;

  Serial.println("Sword locked");

  return true;
}

bool unlock_sword() {
  bool b{0};
  b = digitalRead(PIN_OUT);
  if (unlock_flag == true) {
    Serial.println("Sword already unlocked");
    motor.writeMicroseconds(motor_stop);
    return true;
  }

  b = digitalRead(PIN_OUT);
  unsigned long pMillis{millis()};
  unsigned long cMillis{pMillis};

  Serial.println("Unlocking sword");

  motor.writeMicroseconds(motor_unlock);

  while (b == LOW) {
    if (cMillis - pMillis >= motor_move_time) {
      // if second has passed and motor isnt unlocked, it is stuck raise
      motor.writeMicroseconds(motor_stop);
      error_motor_stuck();
      return false;
    }
    b = digitalRead(PIN_OUT);
    cMillis = millis();

    yield(); // prevent wdt reset
  }

  motor.writeMicroseconds(motor_stop - 300);
  yield();
  delay(50);
  motor.writeMicroseconds(motor_stop - 100);
  yield();
  delay(100);
  motor.writeMicroseconds(motor_stop);

  unlock_flag = true;
  lock_flag = false;

  Serial.println("Sword unlocked");

  return true;
}
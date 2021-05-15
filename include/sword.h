#pragma once
#include "include.h"

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
  delay(200);
  motor.writeMicroseconds(motor_stop + 100);
  yield();
  delay(200);
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
  delay(100);
  motor.writeMicroseconds(motor_stop - 100);
  yield();
  delay(200);
  motor.writeMicroseconds(motor_stop);

  unlock_flag = true;
  lock_flag = false;

  Serial.println("Sword unlocked");

  return true;
}